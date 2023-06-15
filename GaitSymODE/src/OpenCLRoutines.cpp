/*
 *  OpenCLRoutines.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 01/05/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifdef EXPERIMENTAL
#ifdef USE_OPENCL

#include <stdio.h>
#include <cl.h>
#include <vector>
#include <iostream>


#include "OpenCLRoutines.h"

#ifdef USE_QT
#include <QSettings.h>
#include <QDir.h>
#include <QString.h>
#endif


const char *KernelNames[] = {"StressCalc", "StressToTexture"};

const char *KernelStrings[] =
{
        "__kernel void StressCalc(const __global unsigned char *stiffnessMap,\n"
        "                         const __global float *xDistances,\n"
        "                         const __global float *yDistances,\n"
        "                         __global float *stressMap,\n"
        "                         const float t1,\n"
        "                         const float t2,\n"
        "                         const float linearStress,\n"
        "                         __global float *minStress,"
        "                         __global float *maxStress)\n"
        "{\n"
        "\n"
        "    const int nWidth = get_global_size(0);\n"
        "    const int xOut = get_global_id(0);\n"
        "    const int yOut = get_global_id(1);\n"
        "    const int idx = yOut * nWidth + xOut;\n"
        "\n"
        "    if (stiffnessMap[idx] == 0) stressMap[idx] = 0;\n"
        "    else\n"
        "    {\n"
        "        stressMap[idx] = -t1 * xDistances[idx] + t2 * yDistances[idx] + linearStress;\n"
        "        if (stressMap[idx] < minStress[0]) minStress[0] = stressMap[idx];\n"
        "        if (stressMap[idx] > maxStress[0]) maxStress[0] = stressMap[idx];\n"
        "    }\n"
        "}\n"
,

        "__kernel void StressToTexture(const __global unsigned char *stiffnessMap,\n"
        "                              const __global float *stressMap,\n"
        "                              const __global unsigned char *colourMap,\n"
        "                              __global unsigned char *texture,\n"
        "                              const __global float minStressRange,"
        "                              const __global float maxStressRange)\n"
        "{\n"
        "\n"
        "    const int nWidth = get_global_size(0);\n"
        "    const int xOut = get_global_id(0);\n"
        "    const int yOut = get_global_id(1);\n"
        "    const int idx = yOut * nWidth + xOut;\n"
        "    const int idxTex = idx * 4;\n"
        "    int scaledIntensity;"
        "\n"
        "    if (stiffnessMap[idx] == 0)\n"
        "    {\n"
        "        texture[idxTex] = 0;\n"
        "        texture[idxTex + 1] = 0;\n"
        "        texture[idxTex + 2] = 0;\n"
        "        texture[idxTex + 3] = 0;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        if (minStressRange >= maxStressRange) scaledIntensity = 0;\n"
        "        else scaledIntensity = (int)(255.0 * (stressMap[idx] - minStressRange) / (maxStressRange - minStressRange));\n"
        "        texture[idxTex] = colourMap[scaledIntensity];\n"
        "        texture[idxTex + 1] = colourMap[scaledIntensity + 256];\n"
        "        texture[idxTex + 2] = colourMap[scaledIntensity + 512];\n"
        "        texture[idxTex + 3] = 255;\n"
        "    }\n"
        "}\n"
};

#define FREE(ptr, free_val) \
if (ptr != free_val)    \
{                       \
                        free(ptr);          \
                        ptr = free_val;     \
                          }

oclHandleStruct oclHandles;
size_t m_maxWorkgroupSize;

static FILE *gOpenCLLog = 0;
const char *gOpenCLLogFilename = "OpenCL.log";

OpenCLRoutines::OpenCLRoutines()
{
}

/////////////////////////////////////////////////////////////////
// Initialize and shutdown CL
/////////////////////////////////////////////////////////////////

/*
 * Create Context, Device list, Command Queue
 * Load CL file, compile, link CL source
 * Build program and kernel objects
 */
void OpenCLRoutines::InitCL()
{
    cl_device_type  device_type = CL_DEVICE_TYPE_CPU;
    int OpenCLDeviceNumber = 0;
    int OpenCLLog = 0;
    int OpenCLTargetPlatform = 0;
#ifdef USE_QT
    QString OpenCLDeviceType = settings->value("OpenCLDeviceType", QString("CPU")).toString();
    if (OpenCLDeviceType == QString("CPU")) device_type = CL_DEVICE_TYPE_CPU;
    if (OpenCLDeviceType == QString("GPU")) device_type = CL_DEVICE_TYPE_GPU;
    if (OpenCLDeviceType == QString("Accelerator")) device_type = CL_DEVICE_TYPE_ACCELERATOR;
    OpenCLDeviceNumber = settings->value("OpenCLDeviceDeviceNumber", 0).toInt();
    OpenCLTargetPlatform = settings->value("OpenCLTargetPlatform", 0).toInt();
    OpenCLLog = settings->value("OpenCLOpenCLLog", 0).toInt();
    QString openCLLogFilename = QDir::toNativeSeparators(QDir::homePath() + QString("/") + QString(gOpenCLLogFilename));
    if (OpenCLLog) gOpenCLLog = fopen(openCLLogFilename.toUtf8(), "w");
#else
    if (OpenCLLog) gOpenCLLog = fopen(gOpenCLLogFilename, "w");
#endif

    cl_int resultCL;

    oclHandles.context = NULL;
    oclHandles.devices = NULL;
    oclHandles.queue = NULL;
    oclHandles.program = NULL;

    size_t deviceListSize;

    /////////////////////////////////////////////////////////////////
    // Find the available platforms and select one
    /////////////////////////////////////////////////////////////////
    cl_uint numPlatforms;
    cl_platform_id targetPlatform = NULL;

    resultCL = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (resultCL != CL_SUCCESS)
        throw (std::string("InitCL()::Error: Getting number of platforms (clGetPlatformIDs)"));

    if (!(numPlatforms > 0))
        throw (std::string("InitCL()::Error: No platforms found (clGetPlatformIDs)"));

    cl_platform_id* allPlatforms = (cl_platform_id*) malloc(numPlatforms * sizeof(cl_platform_id));

    resultCL = clGetPlatformIDs(numPlatforms, allPlatforms, NULL);
    if (resultCL != CL_SUCCESS)
        throw (std::string("InitCL()::Error: Getting platform ids (clGetPlatformIDs)"));

    /* Select the target platform. Default: first platform */
    targetPlatform = allPlatforms[0];
    for (unsigned int i = 0; i < numPlatforms; i++)
    {
        char pbuff[128];
        resultCL = clGetPlatformInfo( allPlatforms[i],
                                      CL_PLATFORM_VENDOR,
                                      sizeof(pbuff),
                                      pbuff,
                                      NULL);
        if (resultCL != CL_SUCCESS)
            throw (std::string("InitCL()::Error: Getting platform info (clGetPlatformInfo)"));

        if (gOpenCLLog) fprintf(gOpenCLLog, "OpenCL Platform %d %s\n", i, pbuff);

    }

    if (OpenCLTargetPlatform < 0 || OpenCLTargetPlatform >= (int)numPlatforms)
    {
        std::cerr << "Warning: invalid OpenCLTargetPlatform - setting to 0\n";
        if (gOpenCLLog) fprintf(gOpenCLLog, "Warning: invalid OpenCLTargetPlatform - setting to 0\n");
        OpenCLTargetPlatform = 0;
    }

    targetPlatform = allPlatforms[OpenCLTargetPlatform];
    FREE(allPlatforms, NULL);


    /////////////////////////////////////////////////////////////////
    // Create an OpenCL context
    /////////////////////////////////////////////////////////////////
    cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)targetPlatform, 0 };
    oclHandles.context = clCreateContextFromType(cprops,
                                                 device_type,
                                                 NULL,
                                                 NULL,
                                                 &resultCL);

    if ((resultCL != CL_SUCCESS) || (oclHandles.context == NULL))
        throw (std::string("InitCL()::Error: Creating Context (clCreateContextFromType)"));

    /////////////////////////////////////////////////////////////////
    // Detect OpenCL devices
    /////////////////////////////////////////////////////////////////
    /* First, get the size of device list */
    resultCL = clGetContextInfo(oclHandles.context,
                                CL_CONTEXT_DEVICES,
                                0,
                                NULL,
                                &deviceListSize);

    size_t numDevices =  deviceListSize / sizeof(cl_device_id);
    if (gOpenCLLog) fprintf(gOpenCLLog, "%ld Matching Devices Found\n", numDevices);

    if (resultCL != CL_SUCCESS)
        throw(std::string("InitCL()::Error:  Getting Context Info."));
    if (deviceListSize == 0)
        throw(std::string("InitCL()::Error: No devices found."));

    /* Now, allocate the device list */
    oclHandles.devices = (cl_device_id *)malloc(deviceListSize);

    if (oclHandles.devices == 0)
        throw(std::string("InitCL()::Error: Could not allocate memory."));

    /* Next, get the device list data */
    resultCL = clGetContextInfo(oclHandles.context,
                                CL_CONTEXT_DEVICES,
                                deviceListSize,
                                oclHandles.devices,
                                NULL);

    if (resultCL != CL_SUCCESS)
        throw(std::string("InitCL()::Error: Getting Context Info. (device list, clGetContextInfo)"));

    if (OpenCLDeviceNumber < 0 || OpenCLDeviceNumber >= (int)numDevices)
    {
        std::cerr << "Warning: invalid OpenCLDeviceNumber - setting to 0\n";
        if (gOpenCLLog) fprintf(gOpenCLLog, "Warning: invalid OpenCLDeviceNumber - setting to 0\n");
        OpenCLDeviceNumber = 0;
    }

    // print out all the information for the devices
    for (unsigned int i = 0; i < numDevices; i++)
    {
        PrintDevice(oclHandles.devices[i]);
    }
    if (gOpenCLLog) fflush(gOpenCLLog);

    // I'm really interested in workgroup limits
    resultCL = clGetDeviceInfo(oclHandles.devices[OpenCLDeviceNumber], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof m_maxWorkgroupSize, &m_maxWorkgroupSize, NULL);
    if (resultCL != CL_SUCCESS)
        throw(std::string("InitCL()::Error: Getting device Info. (device info, clGetDeviceInfo)"));

    /////////////////////////////////////////////////////////////////
    // Create an OpenCL command queue
    /////////////////////////////////////////////////////////////////
    oclHandles.queue = clCreateCommandQueue(oclHandles.context,
                                            oclHandles.devices[OpenCLDeviceNumber],
                                            0,
                                            &resultCL);

    if ((resultCL != CL_SUCCESS) || (oclHandles.queue == NULL))
        throw(std::string("InitCL()::Creating Command Queue. (clCreateCommandQueue)"));

    /////////////////////////////////////////////////////////////////
    // Load CL string, build CL program object, create CL kernel object
    /////////////////////////////////////////////////////////////////
    for (int i = 0; i < 2; i++)
    {
        const char * source    = KernelStrings[i];
        size_t sourceSize[]    = { strlen(source) };

        oclHandles.program = clCreateProgramWithSource(oclHandles.context,
                                                       1,
                                                       &source,
                                                       sourceSize,
                                                       &resultCL);

        if ((resultCL != CL_SUCCESS) || (oclHandles.program == NULL))
            throw(std::string("InitCL()::Error: Loading Binary into cl_program. (clCreateProgramWithBinary)"));

        resultCL = clBuildProgram(oclHandles.program, 1, oclHandles.devices, NULL, NULL, NULL);

        if ((resultCL != CL_SUCCESS) || (oclHandles.program == NULL))
        {
            std::cerr << "InitCL()::Error: In clBuildProgram" << std::endl;
            if (gOpenCLLog) fprintf(gOpenCLLog, "InitCL()::Error: In clBuildProgram\n");

            size_t length;
            resultCL = clGetProgramBuildInfo(oclHandles.program,
                                             oclHandles.devices[OpenCLDeviceNumber],
                                             CL_PROGRAM_BUILD_LOG,
                                             0,
                                             NULL,
                                             &length);
            if(resultCL != CL_SUCCESS)
                throw(std::string("InitCL()::Error: Getting Program build info(clGetProgramBuildInfo)"));

            char* buffer = (char*)malloc(length);
            resultCL = clGetProgramBuildInfo(oclHandles.program,
                                             oclHandles.devices[OpenCLDeviceNumber],
                                             CL_PROGRAM_BUILD_LOG,
                                             length,
                                             buffer,
                                             NULL);
            if(resultCL != CL_SUCCESS)
                throw(std::string("InitCL()::Error: Getting Program build info(clGetProgramBuildInfo)"));

            std::cerr << buffer << std::endl;
            if (gOpenCLLog) fprintf(gOpenCLLog, "%s\n", buffer);
            free(buffer);

            throw(std::string("InitCL()::Error: Building Program (clBuildProgram)"));
        }

        /* get a kernel object handle for a kernel with the given name */
        cl_kernel kernel = clCreateKernel(oclHandles.program,
                                          KernelNames[i],
                                          &resultCL);

        if ((resultCL != CL_SUCCESS) || (kernel == NULL))
        {
            std::string errorMsg = "InitCL()::Error: Creating Kernel (clCreateKernel) \"StressCalc\"";
            throw(errorMsg);
        }

        oclHandles.kernel.push_back(kernel);
    }
}

/*
 * Release OpenCL resources
 */
void OpenCLRoutines::ReleaseCL()
{
    bool errorFlag = false;

    for (unsigned int nKernel = 0; nKernel < oclHandles.kernel.size(); nKernel++)
    {
        if (oclHandles.kernel[nKernel] != NULL)
        {
            cl_int resultCL = clReleaseKernel(oclHandles.kernel[nKernel]);
            if (resultCL != CL_SUCCESS)
            {
                std::cerr << "ReleaseCL()::Error: In clReleaseKernel" << std::endl;
                if (gOpenCLLog) fprintf(gOpenCLLog, "ReleaseCL()::Error: In clReleaseKernel\n");
                errorFlag = true;
            }
            oclHandles.kernel[nKernel] = NULL;
        }
        oclHandles.kernel.clear();
    }

    if (oclHandles.program != NULL)
    {
        cl_int resultCL = clReleaseProgram(oclHandles.program);
        if (resultCL != CL_SUCCESS)
        {
            std::cerr << "ReleaseCL()::Error: In clReleaseProgram" << std::endl;
            if (gOpenCLLog) fprintf(gOpenCLLog, "ReleaseCL()::Error: In clReleaseProgram");
            errorFlag = true;
        }
        oclHandles.program = NULL;
    }

    if (oclHandles.queue != NULL)
    {
        cl_int resultCL = clReleaseCommandQueue(oclHandles.queue);
        if (resultCL != CL_SUCCESS)
        {
            std::cerr << "ReleaseCL()::Error: In clReleaseCommandQueue" << std::endl;
            if (gOpenCLLog) fprintf(gOpenCLLog, "ReleaseCL()::Error: In clReleaseCommandQueue");
            errorFlag = true;
        }
        oclHandles.queue = NULL;
    }

    FREE(oclHandles.devices, NULL);

    if (oclHandles.context != NULL)
    {
        cl_int resultCL = clReleaseContext(oclHandles.context);
        if (resultCL != CL_SUCCESS)
        {
            std::cerr << "ReleaseCL()::Error: In clReleaseContext" << std::endl;
            if (gOpenCLLog) fprintf(gOpenCLLog, "ReleaseCL()::Error: In clReleaseContext");
            errorFlag = true;
        }
        oclHandles.context = NULL;
    }

    if (gOpenCLLog) fclose(gOpenCLLog);
    gOpenCLLog = 0;

    if (errorFlag) throw(std::string("ReleaseCL()::Error encountered."));
}

// return a user readable error string
// memory allocated needs to be free'd somewhere in the calling routine
char *OpenCLRoutines::GetErrorString(cl_int err)
{
    switch (err)
    {
    case CL_SUCCESS:                          return strdup("Success!");
    case CL_DEVICE_NOT_FOUND:                 return strdup("Device not found.");
    case CL_DEVICE_NOT_AVAILABLE:             return strdup("Device not available");
    case CL_COMPILER_NOT_AVAILABLE:           return strdup("Compiler not available");
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return strdup("Memory object allocation failure");
    case CL_OUT_OF_RESOURCES:                 return strdup("Out of resources");
    case CL_OUT_OF_HOST_MEMORY:               return strdup("Out of host memory");
    case CL_PROFILING_INFO_NOT_AVAILABLE:     return strdup("Profiling information not available");
    case CL_MEM_COPY_OVERLAP:                 return strdup("Memory copy overlap");
    case CL_IMAGE_FORMAT_MISMATCH:            return strdup("Image format mismatch");
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return strdup("Image format not supported");
    case CL_BUILD_PROGRAM_FAILURE:            return strdup("Program build failure");
    case CL_MAP_FAILURE:                      return strdup("Map failure");
    case CL_INVALID_VALUE:                    return strdup("Invalid value");
    case CL_INVALID_DEVICE_TYPE:              return strdup("Invalid device type");
    case CL_INVALID_PLATFORM:                 return strdup("Invalid platform");
    case CL_INVALID_DEVICE:                   return strdup("Invalid device");
    case CL_INVALID_CONTEXT:                  return strdup("Invalid context");
    case CL_INVALID_QUEUE_PROPERTIES:         return strdup("Invalid queue properties");
    case CL_INVALID_COMMAND_QUEUE:            return strdup("Invalid command queue");
    case CL_INVALID_HOST_PTR:                 return strdup("Invalid host pointer");
    case CL_INVALID_MEM_OBJECT:               return strdup("Invalid memory object");
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return strdup("Invalid image format descriptor");
    case CL_INVALID_IMAGE_SIZE:               return strdup("Invalid image size");
    case CL_INVALID_SAMPLER:                  return strdup("Invalid sampler");
    case CL_INVALID_BINARY:                   return strdup("Invalid binary");
    case CL_INVALID_BUILD_OPTIONS:            return strdup("Invalid build options");
    case CL_INVALID_PROGRAM:                  return strdup("Invalid program");
    case CL_INVALID_PROGRAM_EXECUTABLE:       return strdup("Invalid program executable");
    case CL_INVALID_KERNEL_NAME:              return strdup("Invalid kernel name");
    case CL_INVALID_KERNEL_DEFINITION:        return strdup("Invalid kernel definition");
    case CL_INVALID_KERNEL:                   return strdup("Invalid kernel");
    case CL_INVALID_ARG_INDEX:                return strdup("Invalid argument index");
    case CL_INVALID_ARG_VALUE:                return strdup("Invalid argument value");
    case CL_INVALID_ARG_SIZE:                 return strdup("Invalid argument size");
    case CL_INVALID_KERNEL_ARGS:              return strdup("Invalid kernel arguments");
    case CL_INVALID_WORK_DIMENSION:           return strdup("Invalid work dimension");
    case CL_INVALID_WORK_GROUP_SIZE:          return strdup("Invalid work group size");
    case CL_INVALID_WORK_ITEM_SIZE:           return strdup("Invalid work item size");
    case CL_INVALID_GLOBAL_OFFSET:            return strdup("Invalid global offset");
    case CL_INVALID_EVENT_WAIT_LIST:          return strdup("Invalid event wait list");
    case CL_INVALID_EVENT:                    return strdup("Invalid event");
    case CL_INVALID_OPERATION:                return strdup("Invalid operation");
    case CL_INVALID_GL_OBJECT:                return strdup("Invalid OpenGL object");
    case CL_INVALID_BUFFER_SIZE:              return strdup("Invalid buffer size");
    case CL_INVALID_MIP_LEVEL:                return strdup("Invalid mip-map level");
    default:                                  return strdup("Unknown");
    }
}

/*
 * PrintDevice --
 *
 *      Dumps everything about the given device ID.
 *
 * Results:
 *      void.
 */

/*
 * CLErrString --
 *
 *      Utility function that converts an OpenCL status into a human
 *      readable string.
 *
 * Results:
 *      const char * pointer to a static string.
 */

static const char *
CLErrString(cl_int status) {
   static struct { cl_int code; const char *msg; } error_table[] = {
      { CL_SUCCESS, "success" },
      { CL_DEVICE_NOT_FOUND, "device not found", },
      { CL_DEVICE_NOT_AVAILABLE, "device not available", },
      { CL_COMPILER_NOT_AVAILABLE, "compiler not available", },
      { CL_MEM_OBJECT_ALLOCATION_FAILURE, "mem object allocation failure", },
      { CL_OUT_OF_RESOURCES, "out of resources", },
      { CL_OUT_OF_HOST_MEMORY, "out of host memory", },
      { CL_PROFILING_INFO_NOT_AVAILABLE, "profiling not available", },
      { CL_MEM_COPY_OVERLAP, "memcopy overlaps", },
      { CL_IMAGE_FORMAT_MISMATCH, "image format mismatch", },
      { CL_IMAGE_FORMAT_NOT_SUPPORTED, "image format not supported", },
      { CL_BUILD_PROGRAM_FAILURE, "build program failed", },
      { CL_MAP_FAILURE, "map failed", },
      { CL_INVALID_VALUE, "invalid value", },
      { CL_INVALID_DEVICE_TYPE, "invalid device type", },
      { 0, NULL },
   };
   static char unknown[25];
   int ii;

   for (ii = 0; error_table[ii].msg != NULL; ii++) {
      if (error_table[ii].code == status) {
         return error_table[ii].msg;
      }
   }

   snprintf(unknown, sizeof unknown, "unknown error %d", status);
   return unknown;
}

void OpenCLRoutines::PrintDevice(cl_device_id device)
{
#define Warning(...)    if (gOpenCLLog) fprintf(gOpenCLLog, __VA_ARGS__)
#define LONG_PROPS \
    defn(VENDOR_ID), \
            defn(MAX_COMPUTE_UNITS), \
            defn(MAX_WORK_ITEM_DIMENSIONS), \
            defn(MAX_WORK_GROUP_SIZE), \
            defn(PREFERRED_VECTOR_WIDTH_CHAR), \
            defn(PREFERRED_VECTOR_WIDTH_SHORT), \
            defn(PREFERRED_VECTOR_WIDTH_INT), \
            defn(PREFERRED_VECTOR_WIDTH_LONG), \
            defn(PREFERRED_VECTOR_WIDTH_FLOAT), \
            defn(PREFERRED_VECTOR_WIDTH_DOUBLE), \
            defn(MAX_CLOCK_FREQUENCY), \
            defn(ADDRESS_BITS), \
            defn(MAX_MEM_ALLOC_SIZE), \
            defn(IMAGE_SUPPORT), \
            defn(MAX_READ_IMAGE_ARGS), \
            defn(MAX_WRITE_IMAGE_ARGS), \
            defn(IMAGE2D_MAX_WIDTH), \
            defn(IMAGE2D_MAX_HEIGHT), \
            defn(IMAGE3D_MAX_WIDTH), \
            defn(IMAGE3D_MAX_HEIGHT), \
            defn(IMAGE3D_MAX_DEPTH), \
            defn(MAX_SAMPLERS), \
            defn(MAX_PARAMETER_SIZE), \
            defn(MEM_BASE_ADDR_ALIGN), \
            defn(MIN_DATA_TYPE_ALIGN_SIZE), \
            defn(GLOBAL_MEM_CACHELINE_SIZE), \
            defn(GLOBAL_MEM_CACHE_SIZE), \
            defn(GLOBAL_MEM_SIZE), \
            defn(MAX_CONSTANT_BUFFER_SIZE), \
            defn(MAX_CONSTANT_ARGS), \
            defn(LOCAL_MEM_SIZE), \
            defn(ERROR_CORRECTION_SUPPORT), \
            defn(PROFILING_TIMER_RESOLUTION), \
            defn(ENDIAN_LITTLE), \
            defn(AVAILABLE), \
            defn(COMPILER_AVAILABLE),

#define STR_PROPS \
            defn(NAME), \
            defn(VENDOR), \
            defn(PROFILE), \
            defn(VERSION), \
            defn(EXTENSIONS),

#define HEX_PROPS \
            defn(SINGLE_FP_CONFIG), \
            defn(QUEUE_PROPERTIES),


            /* XXX For completeness, it'd be nice to dump this one, too. */
#define WEIRD_PROPS \
            CL_DEVICE_MAX_WORK_ITEM_SIZES,

            static struct { cl_device_info param; const char *name; } longProps[] = {
#define defn(X) { CL_DEVICE_##X, #X }
        LONG_PROPS
#undef defn
        { 0, NULL },
    };
    static struct { cl_device_info param; const char *name; } hexProps[] = {
#define defn(X) { CL_DEVICE_##X, #X }
        HEX_PROPS
#undef defn
        { 0, NULL },
    };
    static struct { cl_device_info param; const char *name; } strProps[] = {
#define defn(X) { CL_DEVICE_##X, #X }
        STR_PROPS
#undef defn
        { CL_DRIVER_VERSION, "DRIVER_VERSION" },
        { 0, NULL },
    };
    cl_int status;
    size_t size;
    char buf[65536];
    long long val; /* Avoids unpleasant surprises for some params */
    int ii;


    for (ii = 0; strProps[ii].name != NULL; ii++) {
        status = clGetDeviceInfo(device, strProps[ii].param, sizeof buf, buf, &size);
        if (status != CL_SUCCESS) {
            Warning("\tdevice[%p]: Unable to get %s: %s!\n",
                    device, strProps[ii].name, CLErrString(status));
            continue;
        }
        if (size > sizeof buf) {
            Warning("\tdevice[%p]: Large %s (%ld bytes)!  Truncating to %ld!\n",
                    device, strProps[ii].name, size, sizeof buf);
        }
        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: %s: %s\n",
               device, strProps[ii].name, buf);
    }
    if (gOpenCLLog) fprintf(gOpenCLLog, "\n");

    status = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof val, &val, NULL);
    if (status == CL_SUCCESS) {
        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: Type: ", device);
        if (val & CL_DEVICE_TYPE_DEFAULT) {
            val &= ~CL_DEVICE_TYPE_DEFAULT;
            if (gOpenCLLog) fprintf(gOpenCLLog, "Default ");
        }
        if (val & CL_DEVICE_TYPE_CPU) {
            val &= ~CL_DEVICE_TYPE_CPU;
            if (gOpenCLLog) fprintf(gOpenCLLog, "CPU ");
        }
        if (val & CL_DEVICE_TYPE_GPU) {
            val &= ~CL_DEVICE_TYPE_GPU;
            if (gOpenCLLog) fprintf(gOpenCLLog, "GPU ");
        }
        if (val & CL_DEVICE_TYPE_ACCELERATOR) {
            val &= ~CL_DEVICE_TYPE_ACCELERATOR;
            if (gOpenCLLog) fprintf(gOpenCLLog, "Accelerator ");
        }
        if (val != 0) {
            if (gOpenCLLog) fprintf(gOpenCLLog, "Unknown (0x%llx) ", val);
        }
        if (gOpenCLLog) fprintf(gOpenCLLog, "\n");
    } else {
        Warning("\tdevice[%p]: Unable to get TYPE: %s!\n",
                device, CLErrString(status));
    }

    status = clGetDeviceInfo(device, CL_DEVICE_EXECUTION_CAPABILITIES,
                             sizeof val, &val, NULL);
    if (status == CL_SUCCESS) {
        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: EXECUTION_CAPABILITIES: ", device);
        if (val & CL_EXEC_KERNEL) {
            val &= ~CL_EXEC_KERNEL;
            if (gOpenCLLog) fprintf(gOpenCLLog, "Kernel ");
        }
        if (val & CL_EXEC_NATIVE_KERNEL) {
            val &= ~CL_EXEC_NATIVE_KERNEL;
            if (gOpenCLLog) fprintf(gOpenCLLog, "Native ");
        }
        if (val) {
            if (gOpenCLLog) fprintf(gOpenCLLog, "Unknown (0x%llx) ", val);
        }
        if (gOpenCLLog) fprintf(gOpenCLLog, "\n");
    } else {
        Warning("\tdevice[%p]: Unable to get EXECUTION_CAPABILITIES: %s!\n",
                device, CLErrString(status));
    }

    status = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
                             sizeof val, &val, NULL);
    if (status == CL_SUCCESS) {
        static const char *cacheTypes[] = { "None", "Read-Only", "Read-Write" };
        static int numTypes = sizeof cacheTypes / sizeof cacheTypes[0];

        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: GLOBAL_MEM_CACHE_TYPE: %s (%lld)\n",
               device, val < numTypes ? cacheTypes[val] : "???", val);
    } else {
        Warning("\tdevice[%p]: Unable to get GLOBAL_MEM_CACHE_TYPE: %s!\n",
                device, CLErrString(status));
    }
    status = clGetDeviceInfo(device,
                             CL_DEVICE_LOCAL_MEM_TYPE, sizeof val, &val, NULL);
    if (status == CL_SUCCESS) {
        static const char *lmemTypes[] = { "???", "Local", "Global" };
        static int numTypes = sizeof lmemTypes / sizeof lmemTypes[0];

        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: CL_DEVICE_LOCAL_MEM_TYPE: %s (%lld)\n",
               device, val < numTypes ? lmemTypes[val] : "???", val);
    } else {
        Warning("\tdevice[%p]: Unable to get CL_DEVICE_LOCAL_MEM_TYPE: %s!\n",
                device, CLErrString(status));
    }

    for (ii = 0; hexProps[ii].name != NULL; ii++) {
        status = clGetDeviceInfo(device, hexProps[ii].param, sizeof val, &val, &size);
        if (status != CL_SUCCESS) {
            Warning("\tdevice[%p]: Unable to get %s: %s!\n",
                    device, hexProps[ii].name, CLErrString(status));
            continue;
        }
        if (size > sizeof val) {
            Warning("\tdevice[%p]: Large %s (%ld bytes)!  Truncating to %ld!\n",
                    device, hexProps[ii].name, size, sizeof val);
        }
        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: %s: 0x%llx\n",
               device, hexProps[ii].name, val);
    }
    if (gOpenCLLog) fprintf(gOpenCLLog, "\n");

    for (ii = 0; longProps[ii].name != NULL; ii++) {
        status = clGetDeviceInfo(device, longProps[ii].param, sizeof val, &val, &size);
        if (status != CL_SUCCESS) {
            Warning("\tdevice[%p]: Unable to get %s: %s!\n",
                    device, longProps[ii].name, CLErrString(status));
            continue;
        }
        if (size > sizeof val) {
            Warning("\tdevice[%p]: Large %s (%ld bytes)!  Truncating to %ld!\n",
                    device, longProps[ii].name, size, sizeof val);
        }
        if (gOpenCLLog) fprintf(gOpenCLLog, "\tdevice[%p]: %s: %lld\n",
               device, longProps[ii].name, val);
    }
}

// return the precalculated max workgroup size;
size_t OpenCLRoutines::GetMaxWorkgroupSize()
{
    return m_maxWorkgroupSize;
}

#endif
#endif
