/*
 *  OpenCLRoutines.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 01/05/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef OPENCLROUTINES_H
#define OPENCLROUTINES_H

#ifdef EXPERIMENTAL
#ifdef USE_OPENCL

#include <cl.h>

struct oclHandleStruct
{
    cl_context              context;
    cl_device_id            *devices;
    cl_command_queue        queue;
    cl_program              program;
    std::vector<cl_kernel>  kernel;
};



class OpenCLRoutines
{
public:
    OpenCLRoutines();

    static void InitCL();
    static void ReleaseCL();
    static char *GetErrorString(cl_int err);
    static void PrintDevice(cl_device_id device);
    static size_t GetMaxWorkgroupSize();

protected:

};

#endif
#endif

#endif // OPENCLROUTINES_H
