/*
 *  FixedJoint.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 20/09/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#include <ode/ode.h>

#include "FixedJoint.h"
#include "DataFile.h"
#include "Body.h"
#include "Simulation.h"
#include "Util.h"
#include "TIFFWrite.h"

// Simulation global
extern Simulation *gSimulation;

#ifdef EXPERIMENTAL

#ifdef USE_OPENGL
#include "GLUtils.h"
extern int gAxisFlag;
#endif

#ifdef USE_OPENCL
#include <cl.h>
#include "OpenCLRoutines.h"
extern oclHandleStruct oclHandles;
#endif

extern int g_UseOpenCL;

#endif

FixedJoint::FixedJoint(dWorldID worldID) : Joint()
{
    m_JointID = dJointCreateFixed(worldID, 0);
    dJointSetData(m_JointID, this);

    dJointSetFeedback(m_JointID, &m_JointFeedback);

#ifdef EXPERIMENTAL

    // now the bits for stress calculations
    m_stiffness = 0;
    m_xDistances = 0;
    m_yDistances = 0;
    m_stress = 0;
    m_nx = 0;
    m_ny = 0;
    m_dx = 0;
    m_dy = 0;
    m_Ix = 0;
    m_Iy = 0;
    m_Ixy = 0;
    m_area = 0;
    m_xOrigin = 0;
    m_yOrigin = 0;
    m_minStress = 0;
    m_maxStress = 0;
    m_minStress = 0;
    m_maxStress = 0;
    m_stressCalculationType = none;
    m_vectorList = 0;
    m_stressLimit = -1;
    m_stressListMin = 0;
    m_stressListMax = 0;
    m_stressListIndex = 0;
    m_stressWindow = 0;
    m_timeAveragedMaxStress = 0;
    m_timeAverageTotalMaxStress = 0;
    m_timeAveragedMinStress = 0;
    m_timeAverageTotalMinStress = 0;
    m_dumpCount = 0;
    m_stressCompleteList = 0;
    m_stressCompleteTotal = 0;

    m_lowRange = 0;
    m_highRange = 0;
    m_colourMap = 0;

#ifdef USE_OPENGL
    m_LastDisplayTime = -1;
    m_texture = 0;
    glGenTextures(1, &m_textureID);
#endif

#ifdef USE_OPENCL
    m_stiffnessBuffer = 0;
    m_xDistanceBuffer = 0;
    m_yDistanceBuffer = 0;
    m_stressBuffer = 0;
    m_minStressBuffer = 0;
    m_maxStressBuffer = 0;
    m_textureBuffer = 0;
    m_colourMapBuffer = 0;
#endif

#endif

}

FixedJoint::~FixedJoint()
{
#ifdef EXPERIMENTAL

    if (m_stiffness) delete m_stiffness;
    if (m_xDistances) delete [] m_xDistances;
    if (m_yDistances) delete [] m_yDistances;
    if (m_stress) delete [] m_stress;
    if (m_vectorList) delete [] m_vectorList;
    if (m_stressListMin) delete [] m_stressListMin;
    if (m_stressListMax) delete [] m_stressListMax;
    if (m_stressCompleteList) delete [] m_stressCompleteList;
    if (m_stressCompleteTotal) delete [] m_stressCompleteTotal;
    if (m_colourMap) delete [] m_colourMap;

#ifdef USE_OPENGL
    glDeleteTextures(1, &m_textureID);
    if (m_texture) delete [] m_texture;
#endif

#ifdef USE_OPENCL
    cl_int   resultCL;
    if (m_stiffnessBuffer) resultCL = clReleaseMemObject(m_stiffnessBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_xDistanceBuffer) resultCL = clReleaseMemObject(m_xDistanceBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_yDistanceBuffer) resultCL = clReleaseMemObject(m_yDistanceBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_stressBuffer) resultCL = clReleaseMemObject(m_stressBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_stressBuffer) resultCL = clReleaseMemObject(m_minStressBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_stressBuffer) resultCL = clReleaseMemObject(m_maxStressBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_textureBuffer) resultCL = clReleaseMemObject(m_textureBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
    if (m_colourMapBuffer) resultCL = clReleaseMemObject(m_colourMapBuffer);
    if (resultCL != CL_SUCCESS) std::cerr << "Error in FixedJoint::~FixedJoint at line " << __LINE__ << "\n";
#endif

#endif
}

void FixedJoint::SetFixed()
{
    dJointSetFixed (m_JointID);
}

#ifdef EXPERIMENTAL
// this is the part where we calculate the stress map
// SetCrossSection needs to be called first
// note for this to work the centre of the fixed joint needs to be the centroid of the cross section area
void FixedJoint::CalculateStress()
{
    // first of all we need to convert the forces and torques into the joint local coordinate system

    // the force feedback is at the CM for fixed joints
    // first we need to move it to the joint position

    // calculate the offset of the stress position from the CM
    dVector3 result;
    dBodyVectorToWorld (this->GetBody1()->GetBodyID(), m_StressOrigin.x, m_StressOrigin.y, m_StressOrigin.z, result);
    pgd::Vector worldStressOffset(result[0], result[1], result[2]);

    // now the linear components of m_JointFeedback will generate a torque if applied at this position
    // torque = r x f
    pgd::Vector forceCM(m_JointFeedback.f1[0], m_JointFeedback.f1[1], m_JointFeedback.f1[2]);
    pgd::Vector addedTorque = worldStressOffset ^ forceCM;

    pgd::Vector torqueCM(m_JointFeedback.t1[0], m_JointFeedback.t1[1], m_JointFeedback.t1[2]);
    pgd::Vector torqueStressOrigin = torqueCM - addedTorque;

    // now rotate new values to stress based coordinates
    const double *q = dBodyGetQuaternion (this->GetBody1()->GetBodyID());
    pgd::Quaternion bodyOrientation(q[0], q[1], q[2], q[3]);
    m_torqueStressCoords = pgd::QVRotate(m_StressOrientation, pgd::QVRotate(bodyOrientation, torqueStressOrigin));
    m_forceStressCoords = pgd::QVRotate(m_StressOrientation, pgd::QVRotate(bodyOrientation, forceCM));

    if (m_stressCalculationType == beam)
    {

        // torques x and y correspond to bending and z corresponds to torsion
        // forces x and y correspond to shear and z corresponds to compressions/tension

        // now we use the cross section bitmap to calculate the loading

        // first linear

        double linearStress = m_forceStressCoords.z / m_area;

        // now add rotational

        // Stress in a beam
        // The general form of the classic bending formula for a beam in co-ordinate system having origin located at the neutral axis of the beam is (Pilkey 2002, p. 17):
        // Pilkey, Walter D. (2002). Analysis and Design of Elastic Beams. John Wiley & Sons, Inc.. ISBN 0-471-38152-7

        // sigma = -[(MyIx + MxIxy)/(IxIy - Ixy^2)]x + [(MxIy + MyIxy)/(IxIy - Ixy^2)y

        // sigma = the normal stress in the beam due to bending
        // x = the perpendicular distance to the centroidal y-axis
        // y = the perpendicular distance to the centroidal x-axis
        // My = the bending moment about the y-axis
        // Mx = the bending moment about the x-axis
        // Ix = the second moment of area about x-axis
        // Iy = the second moment of area about y-axis
        // Ixy = the product moment of area
        // If the coordinate system is chosen to give a product moment of area equal to zero, the formula simplifies to:

        // sigma = -(My/Iy)x + (Mx/Ix)y

        // If additionally the beam is only subjected to bending about one axis, the formula simplifies further:

        // sigma = (Mx/Ix)y

        // for the x and y (bending) components
        // for each element in the stiffness array
        // stress = (torque / second moment of area) * moment arm

        double My = m_torqueStressCoords.y;
        double Mx = m_torqueStressCoords.x;
        // precalculate invariant bits of the formula
        double t1 = (My * m_Ix + Mx * m_Ixy)/(m_Ix * m_Iy - m_Ixy * m_Ixy);
        double t2 = (Mx * m_Iy + My * m_Ixy)/(m_Ix * m_Iy - m_Ixy * m_Ixy);
        double *xDistancePtr = m_xDistances;
        double *yDistancePtr = m_yDistances;

        double *stressPtr = m_stress;
        int i;
        if (g_UseOpenCL == false)
        {
            m_minStress = DBL_MAX;
            m_maxStress = -DBL_MAX;
            for (i = 0; i < m_nActivePixels; i++)
            {
                *stressPtr = -t1 * (*xDistancePtr) + t2 * (*yDistancePtr) + linearStress;
                if (*stressPtr > m_maxStress) m_maxStress = *stressPtr;
                if (*stressPtr < m_minStress) m_minStress = *stressPtr;
                stressPtr++;
                xDistancePtr++;
                yDistancePtr++;
            }
        }
        else
        {
#ifdef USE_OPENCL_BROKEN

            cl_int resultCL;
            try
            {
                // set the kernel arguments
                resultCL = clSetKernelArg(oclHandles.kernel[0], 0, sizeof(cl_mem), (void *)&m_stiffnessBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 1, sizeof(cl_mem), (void *)&m_xDistanceBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 2, sizeof(cl_mem), (void *)&m_yDistanceBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 3, sizeof(cl_mem), (void *)&m_stressBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 4, sizeof(float), (void *)&t1);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 5, sizeof(float), (void *)&t2);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 6, sizeof(float), (void *)&linearStress);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 7, sizeof(cl_mem), (void *)&m_minStressBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;
                resultCL = clSetKernelArg(oclHandles.kernel[0], 8, sizeof(cl_mem), (void *)&m_maxStressBuffer);
                if (resultCL != CL_SUCCESS) throw __LINE__;

                // and do the kernal
                cl_event events[1];
                const int WORK_DIM = 2;
                size_t globalNDWorkSize[WORK_DIM];  //Total number of work items
                size_t localNDWorkSize[WORK_DIM];   //Work items in each work-group
                globalNDWorkSize[0] = m_nx;
                globalNDWorkSize[1] = m_ny;
                if (OpenCLRoutines::GetMaxWorkgroupSize() < 256)
                {
                    localNDWorkSize[0] = 1;
                    localNDWorkSize[1] = 1;
                }
                else
                {
                    localNDWorkSize[0] = 16;
                    localNDWorkSize[1] = 16;
                }

                resultCL = clEnqueueNDRangeKernel(oclHandles.queue,
                                                  oclHandles.kernel[0],
                                                  WORK_DIM,
                                                  NULL,
                                                  globalNDWorkSize,
                                                  localNDWorkSize,
                                                  0, NULL,
                                                  &events[0]);

                if (resultCL != CL_SUCCESS) throw __LINE__;

                /* wait for the kernel call to finish execution */
                /* (note clFinish might be an alternative ) */
                resultCL = clWaitForEvents(1, &events[0]);
                if (resultCL != CL_SUCCESS) throw __LINE__;

                resultCL = clEnqueueReadBuffer(oclHandles.queue,            // cl_command_queue command_queue
                                               m_stressBuffer,              // cl_mem buffer
                                               CL_TRUE,                     // cl_bool blocking_read
                                               0,                           // size_t offset
                                               sizeof(float) * m_nx * m_ny, // size_t cb
                                               m_stress,                    // void *ptr
                                               0,                           // cl_uint num_events_in_wait_list
                                               NULL,                        // const cl_event *event_wait_list
                                               NULL);                       // cl_event *event

                resultCL = clEnqueueReadBuffer(oclHandles.queue,            // cl_command_queue command_queue
                                               m_minStressBuffer,           // cl_mem buffer
                                               CL_TRUE,                     // cl_bool blocking_read
                                               0,                           // size_t offset
                                               sizeof(float),               // size_t cb
                                               &m_minStress,                // void *ptr
                                               0,                           // cl_uint num_events_in_wait_list
                                               NULL,                        // const cl_event *event_wait_list
                                               NULL);                       // cl_event *event

                resultCL = clEnqueueReadBuffer(oclHandles.queue,            // cl_command_queue command_queue
                                               m_maxStressBuffer,           // cl_mem buffer
                                               CL_TRUE,                     // cl_bool blocking_read
                                               0,                           // size_t offset
                                               sizeof(float),               // size_t cb
                                               &m_maxStress,                // void *ptr
                                               0,                           // cl_uint num_events_in_wait_list
                                               NULL,                        // const cl_event *event_wait_list
                                               NULL);                       // cl_event *event

            }

            catch (int e)
            {
                char *errStr = OpenCLRoutines::GetErrorString(resultCL);
                std::cerr << "FixedJoint::CalculateStress error in line " << e << "\n" << errStr << "\n";
                free(errStr);
            }
#endif
        }
    }
    else if (m_stressCalculationType == spring)
    {
        m_torqueScalar = m_torqueStressCoords.Magnitude();
        m_torqueAxis = m_torqueStressCoords / m_torqueScalar;

        // assuming all the springs are the same then
        pgd::Vector forcePerSpring1 = m_forceStressCoords / m_nActivePixels;

        // but for the torque we need to find the total torsional springiness
        double *xDistancePtr = m_xDistances;
        double *yDistancePtr = m_yDistances;
        double totalNominalTorque = 0;
        int i;

        for (i = 0; i < m_nActivePixels; i++)
        {
            pgd::Vector p(*xDistancePtr, *yDistancePtr, 0);
            pgd::Vector closestPoint = m_torqueAxis * (m_torqueAxis * p);
            pgd::Vector r = p - closestPoint;
            double distance2 = r.Magnitude2();
            double distance = sqrt(distance2);
            if (distance2 > 1e-10)
            {
                pgd::Vector direction = m_torqueAxis ^ r;
                direction.Normalize();
                pgd::Vector forcePerSpring2 =  direction * distance; // force per spring is proportional to the perpendicular distance
                totalNominalTorque += distance2; // but the torque per spring is proportional to the perpendicular distance squared
                m_vectorList[i] = forcePerSpring2;
            }
            else
            {
                m_vectorList[i] = pgd::Vector(0, 0, 0);
            }
            xDistancePtr++;
            yDistancePtr++;
        }

        double torqueScale = m_torqueScalar / totalNominalTorque; // this will make the total torque produced by the springs add up to the actual torque
        m_minStress = DBL_MAX;
        m_maxStress = -DBL_MAX;
        double *stressPtr = m_stress;
        double dArea = m_dx * m_dy;
        for (i = 0; i < m_nActivePixels; i++)
        {
            //std::cerr << m_vectorList[i].x << " " << m_vectorList[i].y << " " << m_vectorList[i].z << "\n";
            m_vectorList[i] = (m_vectorList[i] * torqueScale) + forcePerSpring1;
            *stressPtr = m_vectorList[i].Magnitude() / dArea;
            if (*stressPtr > m_maxStress) m_maxStress = *stressPtr;
            if (*stressPtr < m_minStress) m_minStress = *stressPtr;
            stressPtr++;
        }

//#define SANITY_CHECK
#ifdef SANITY_CHECK
        // check that my forces and my torqes add up
        pgd::Vector totalForce;
        pgd::Vector totalTorque;
        i = 0;
        ptr = m_stiffness;
        for (iy = 0; iy < m_ny; iy++)
        {
            for (ix = 0; ix < m_nx; ix++)
            {
                if (*ptr)
                {
                    totalForce += m_vectorList[i];

                    pgd::Vector p(((ix) + 0.5) * m_dx - m_xOrigin, ((iy) + 0.5) * m_dy - m_yOrigin, 0);
                    pgd::Vector closestPoint = torqueAxis * (torqueAxis * p);
                    pgd::Vector r = p - closestPoint;
                    pgd::Vector torque = r ^ m_vectorList[i];
                    std::cerr << "torque " << torque.x << " " << torque.y << " " << torque.z << "\n";
                    totalTorque += torque;
                    i++;
                }
                ptr++;
            }
        }
        std::cerr << "m_forceStressCoords " << m_forceStressCoords.x << " " << m_forceStressCoords.y << " " << m_forceStressCoords.z << "\n";
        std::cerr << "totalForce " << totalForce.x << " " << totalForce.y << " " << totalForce.z << "\n";
        std::cerr << "m_torqueStressCoords " << m_torqueStressCoords.x << " " << m_torqueStressCoords.y << " " << m_torqueStressCoords.z << "\n";
        std::cerr << "totalTorque " << totalTorque.x << " " << totalTorque.y << " " << totalTorque.z << "\n";
        std::cerr << "m_torqueStressCoords.Magnitude " << m_torqueStressCoords.Magnitude() << " " << "totalTorque.Magnitude " << totalTorque.Magnitude() << "\n";
#endif
    }

    // update the stress list
    if (m_stressListMin)
    {
        m_stressListIndex++;
        if (m_stressListIndex >= m_stressWindow) m_stressListIndex = 0;

        m_timeAverageTotalMaxStress -= m_stressListMax[m_stressListIndex];
        m_timeAverageTotalMaxStress += m_maxStress;
        m_stressListMax[m_stressListIndex] = m_maxStress;
        m_timeAveragedMaxStress = m_timeAverageTotalMaxStress / m_stressWindow;

        m_timeAverageTotalMinStress -= m_stressListMin[m_stressListIndex];
        m_timeAverageTotalMinStress += m_minStress;
        m_stressListMin[m_stressListIndex] = m_minStress;
        m_timeAveragedMinStress = m_timeAverageTotalMinStress / m_stressWindow;
    }



}


// this is where we set the cross section and precalculate the second moment of area
// the cross section array is a unsigned char image with origin at bottom left (standard raster origin will need to have y reversed)
// it scans row first and then vertically
// nx, ny are the dimensions of the array
// dx, dy are the real world sizes of each pixel in the array
// stiffness array ownership is taken over by FixedJoint and not copied
void FixedJoint::SetCrossSection(unsigned char *stiffness, int nx, int ny, double dx, double dy)
{
    int ix, iy;
    unsigned char *ptr;
    double xsum = 0;
    double ysum = 0;

    m_dx = dx;
    m_dy = dy;
    m_nx = nx;
    m_ny = ny;

    if (m_stiffness) delete m_stiffness;
    m_stiffness = stiffness;
    ptr = m_stiffness;

    // calculate centre of area
    m_nActivePixels = 0;
    for (iy = 0; iy < m_ny; iy++)
    {
        for (ix = 0; ix < m_nx; ix++)
        {
            if (*ptr)
            {
                m_nActivePixels++;
                xsum += (double(ix) + 0.5f) * m_dx;
                ysum += (double(iy) + 0.5f) * m_dy;
            }
            ptr++;
        }
    }
    m_xOrigin = xsum / m_nActivePixels;
    m_yOrigin = ysum / m_nActivePixels;

    // now calculate second moment of area and distances from the origin

    ptr = m_stiffness;
    double dArea = m_dx * m_dy;
    m_area = m_nActivePixels * dArea;
    if (m_xDistances) delete [] m_xDistances;
    m_xDistances = new double[m_nActivePixels];
    double *xDistancePtr = m_xDistances;
    if (m_yDistances) delete [] m_yDistances;
    m_yDistances = new double[m_nActivePixels];
    double *yDistancePtr = m_yDistances;
    m_Ix = 0;
    m_Iy = 0;
    m_Ixy = 0;
    for (iy = 0; iy < m_ny; iy++)
    {
        for (ix = 0; ix < m_nx; ix++)
        {
            if (*ptr)
            {
                *xDistancePtr = (double(ix) + 0.5f) * m_dx - m_xOrigin;
                *yDistancePtr = (double(iy) + 0.5f) * m_dy - m_yOrigin;
                // second moments of area and product moment of area
                // Pilkey, Walter D. (2002). Analysis and Design of Elastic Beams. John Wiley & Sons, Inc.. ISBN 0-471-38152-7
                // Ix = sum(y2 * area)
                // Iy = sum(x2 * area)
                // Ixy = sum(xy * area)
                m_Ix += (*yDistancePtr) * (*yDistancePtr) * dArea;
                m_Iy += (*xDistancePtr) * (*xDistancePtr) * dArea;
                m_Ixy += (*xDistancePtr) * (*yDistancePtr) * dArea;
                xDistancePtr++;
                yDistancePtr++;
            }
            ptr++;
        }
    }

    // allocate the vector list
    if (m_vectorList) delete [] m_vectorList;
    m_vectorList = new pgd::Vector[m_nActivePixels];

    if (m_stress) delete [] m_stress;
    m_stress = new double[m_nActivePixels];

#ifdef USE_OPENGL
    m_texture = new unsigned char[m_nx * m_ny * 4];
    memset(m_texture, 0, m_nx * m_ny * 4);
    m_colourMap = new unsigned char[256 * 3];
    double r;
    for ( int i = 0; i < 256; i++)
    {
        r = ((double)i / 255.0) * 2 * M_PI;
        m_colourMap[i] = (int)(255.0 * (sin(r / 2 + M_PI / 2) + 1) / 2);
        m_colourMap[i + 256] = (int)(255.0 * (sin(r - M_PI / 2) + 1) / 2);
        m_colourMap[i + 512] = (int)(255.0 * (sin(r / 2 + 3 * M_PI / 2) + 1) / 2);
    }

#endif

#ifdef USE_OPENCL_BROKEN
    cl_int resultCL;
    // Create OpenCL memory buffers
    try
    {
        m_stiffnessBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_uchar) * m_nx * m_ny,
                                           m_stiffness,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_stiffnessBuffer == NULL)) throw __LINE__;

        m_xDistanceBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_float) * m_nx * m_ny,
                                           m_xDistances,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_xDistanceBuffer == NULL)) throw __LINE__;

        m_yDistanceBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_float) * m_nx * m_ny,
                                           m_yDistances,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_yDistanceBuffer == NULL)) throw __LINE__;

        m_stressBuffer = clCreateBuffer(oclHandles.context,
                                        CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                                        sizeof(cl_float) * m_nx * m_ny,
                                        m_stress,
                                        &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_stressBuffer == NULL)) throw __LINE__;

        m_minStressBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_float),
                                           &m_minStress,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_minStressBuffer == NULL)) throw __LINE__;

        m_maxStressBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_float),
                                           &m_maxStress,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_maxStressBuffer == NULL)) throw __LINE__;

#if USE_OPENGL
        m_textureBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_uchar) * m_nx * m_ny * 4,
                                           m_texture,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_textureBuffer == NULL)) throw __LINE__;

        m_colourMapBuffer = clCreateBuffer(oclHandles.context,
                                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                           sizeof(cl_uchar) * 256 * 3,
                                           m_colourMap,
                                           &resultCL);
        if ((resultCL != CL_SUCCESS) || (m_colourMapBuffer == NULL)) throw __LINE__;
#endif

    }
    catch(int e)
    {
        char *errStr = OpenCLRoutines::GetErrorString(resultCL);
        std::cerr << "FixedJoint::SetCrossSection memory allocation error in line " << e << "\n" << errStr << "\n";
        free(errStr);
    }
#endif

}

void FixedJoint::SetStressOrigin(double x, double y, double z)
{
    m_StressOrigin.x = x;
    m_StressOrigin.y = y;
    m_StressOrigin.z = z;
}


// parses the position allowing a relative position specified by BODY ID
// x y z - world coordinates
// bodyName x y z - position relative to bodyName local coordinate system
void FixedJoint::SetStressOrigin(const char *buf)
{
    int i;
    int l = strlen(buf);
    char *lBuf = (char *)alloca((l + 1) * sizeof(char));
    char **lBufPtrs = (char **)alloca(l * sizeof(char *));
    dVector3 pos, result;

    strcpy(lBuf, buf);
    int count = DataFile::ReturnTokens(lBuf, lBufPtrs, l);
    if (count < 3)
    {
        std::cerr << "Error in FixedJoint::SetStressOrigin\n";
        return; // error condition
    }

    if (isalpha((int)*lBufPtrs[0]) == 0)
    {
        for (i = 0; i < 3; i++) pos[i] = strtod(lBufPtrs[i], 0);
        dBodyGetPosRelPoint(this->GetBody1()->GetBodyID(), pos[0], pos[1], pos[2], result); // convert from world to body
        SetStressOrigin(result[0], result[1], result[2]);
        return;
    }

    if (count < 4)
    {
        std::cerr << "Error in FixedJoint::SetStressOrigin\n";
        return; // error condition
    }
    Body *theBody = gSimulation->GetBody(lBufPtrs[0]);
    if (theBody == 0)
    {
        if (strcmp(lBufPtrs[0], "World") == 0)
        {
            for (i = 0; i < 3; i++) pos[i] = strtod(lBufPtrs[i + 1], 0);
            dBodyGetPosRelPoint(this->GetBody1()->GetBodyID(), pos[0], pos[1], pos[2], result); // convert from world to body
            SetStressOrigin(result[0], result[1], result[2]);
            return;
        }
        else
        {
            std::cerr << "Error in FixedJoint::SetStressOrigin\n";
            return; // error condition
        }
    }
    for (i = 0; i < 3; i++) pos[i] = strtod(lBufPtrs[i + 1], 0);
    dBodyGetRelPointPos(theBody->GetBodyID(), pos[0], pos[1], pos[2], result); // convert from body to world
    dBodyGetPosRelPoint(this->GetBody1()->GetBodyID(), result[0], result[1], result[2], pos); // convert from world to body
    SetStressOrigin(pos[0], pos[1], pos[2]);
}

void FixedJoint::SetStressOrientation(double q0, double q1, double q2, double q3)
{
    m_StressOrientation.n = q0;
    m_StressOrientation.v.x = q1;
    m_StressOrientation.v.y = q2;
    m_StressOrientation.v.z = q3;
    m_StressOrientation.Normalize(); // this is the safest option
}

// parses the quaternion allowing a relative position specified by BODY ID
// note quaternion is (qs,qx,qy,qz)
// s x y z - world coordinates
// bodyName s x y z - position relative to bodyName local coordinate system
void FixedJoint::SetStressOrientation(const char *buf)
{
    int l = strlen(buf);
    char *lBuf = (char *)alloca((l + 1) * sizeof(char));
    char **lBufPtrs = (char **)alloca(l * sizeof(char *));
    dQuaternion quaternion;

    strcpy(lBuf, buf);
    int count = DataFile::ReturnTokens(lBuf, lBufPtrs, l);
    if (count < 4)
    {
        std::cerr << "Error in FixedJoint::SetStressOrientation\n";
        return; // error condition
    }


    if (isalpha((int)*lBufPtrs[0]) == 0)
    {
        const double *q = dBodyGetQuaternion(this->GetBody1()->GetBodyID());
        pgd::Quaternion qBody(q[0], q[1], q[2], q[3]);
        Util::GetQuaternion(&lBufPtrs[0], quaternion);
        pgd::Quaternion qWorld(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
        pgd::Quaternion qLocal = ~qBody * qWorld;
        SetStressOrientation(qLocal.n, qLocal.v.x, qLocal.v.y, qLocal.v.z);
        return;
    }

    if (count < 5)
    {
        std::cerr << "Error in FixedJoint::SetStressOrientation\n";
        return; // error condition
    }
    Body *theBody = gSimulation->GetBody(lBufPtrs[0]);
    if (theBody == 0)
    {
        if (strcmp(lBufPtrs[0], "World") == 0)
        {
            const double *q = dBodyGetQuaternion(this->GetBody1()->GetBodyID());
            pgd::Quaternion qBody(q[0], q[1], q[2], q[3]);
            Util::GetQuaternion(&lBufPtrs[1], quaternion);
            pgd::Quaternion qWorld(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
            pgd::Quaternion qLocal = ~qBody * qWorld;
            SetStressOrientation(qLocal.n, qLocal.v.x, qLocal.v.y, qLocal.v.z);
            return;
        }
        else
        {
            std::cerr << "Error in FixedJoint::SetStressOrientation\n";
            return; // error condition
        }
    }

    // first get world quaternion
    const double *q2 = theBody->GetQuaternion();
    pgd::Quaternion qBody1(q2[0], q2[1], q2[2], q2[3]);
    Util::GetQuaternion(&lBufPtrs[1], quaternion);
    pgd::Quaternion qBody2(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
    pgd::Quaternion qWorld = qBody1 * qBody2;

    // then set the local quaternion
    const double *q = dBodyGetQuaternion(this->GetBody1()->GetBodyID());
    pgd::Quaternion qBody(q[0], q[1], q[2], q[3]);
    pgd::Quaternion qLocal = ~qBody * qWorld;
    SetStressOrientation(qLocal.n, qLocal.v.x, qLocal.v.y, qLocal.v.z);

}

void FixedJoint::SetStressLimit(double stressLimit, int stressWindow)
{
    m_stressLimit = stressLimit;
    if (m_stressListMin) delete [] m_stressListMin;
    if (m_stressListMax) delete [] m_stressListMax;
    m_stressWindow = stressWindow;
    m_stressListMin = new double[m_stressWindow];
    m_stressListMax = new double[m_stressWindow];
    memset(m_stressListMin, 0, m_stressWindow * sizeof(double));
    memset(m_stressListMax, 0, m_stressWindow * sizeof(double));
    m_stressListIndex = m_stressWindow;
}


bool FixedJoint::CheckStressAbort()
{
    if (m_stressListMin == 0) return false;

    if (m_timeAveragedMaxStress > m_stressLimit) return true;
    if (m_timeAveragedMinStress < -m_stressLimit) return true;
    return false;
}

void FixedJoint::Update()
{
    if (m_stressCalculationType != none) CalculateStress();
}

void FixedJoint::Dump()
{
    if (m_Dump == false) return;

    if (m_FirstDump)
    {
        m_FirstDump = false;
        if (m_DumpStream == 0)
        {
            if (m_Name.size() == 0) std::cerr << "FixedJoint::Dump error: can only dump a named object\n";
            std::string filename(m_Name);
            filename.append(".dump");
            m_DumpStream = new std::ofstream(filename.c_str());
            m_DumpStream->precision(17);

            // allocate the big list for the bitmap
            m_stressCompleteList = new double[m_nActivePixels * m_stressWindow];
            m_stressCompleteTotal = new double[m_nActivePixels];
            memset(m_stressCompleteList, 0, m_nActivePixels * m_stressWindow * sizeof(double));
            memset(m_stressCompleteTotal, 0, m_nActivePixels * sizeof(double));
        }

        if (m_DumpStream)
        {
            if (m_stiffness == 0)
            {
                *m_DumpStream << "Time\tXP\tYP\tZP\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFX2\tFY2\tFZ2\tTX2\tTY2\tTZ2\n";
            }
            else
            {
                if (m_stressCalculationType == beam)
                {
                    *m_DumpStream << "XOrigin\tYOrigin\tArea\tIx\tIy\tIxy\n";
                    *m_DumpStream << m_xOrigin << "\t" << m_yOrigin << "\t" << m_area <<
                                     "\t" << m_Ix << "\t" << m_Iy << "\t" << m_Ixy <<
                                     "\n";
                    *m_DumpStream << "Time\tXP\tYP\tZP\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFXS1\tFYS1\tFZS1\tTXS1\tTYS1\tTZS1\tMinStress\tMaxStress\tTimeAveragedMinStress\tTimeAveragedMaxStress\n";
                }
                else if (m_stressCalculationType == spring)
                {
                    *m_DumpStream << "XOrigin\tYOrigin\tArea\tIx\tIy\tIxy\n";
                    *m_DumpStream << m_xOrigin << "\t" << m_yOrigin << "\t" << m_area <<
                                     "\t" << m_Ix << "\t" << m_Iy << "\t" << m_Ixy <<
                                     "\n";
                    *m_DumpStream << "Time\tXP\tYP\tZP\tFX1\tFY1\tFZ1\tTX1\tTY1\tTZ1\tFXS1\tFYS1\tFZS1\tTXSA1\tTYSA1\tTZSA1\tTorqueScalar\tMinStress\tMaxStress\tTimeAveragedMinStress\tTimeAveragedMaxStress\n";
                }

            }
        }
    }


    if (m_DumpStream)
    {
        if (m_stiffness == 0)
        {
            const double *p = dBodyGetPosition(this->GetBody1()->GetBodyID());

            *m_DumpStream << gSimulation->GetTime() << "\t" << p[0] << "\t" << p[1] << "\t" << p[2] << "\t" <<
                             m_JointFeedback.f1[0] << "\t" << m_JointFeedback.f1[1] << "\t" << m_JointFeedback.f1[2] << "\t" <<
                             m_JointFeedback.t1[0] << "\t" << m_JointFeedback.t1[1] << "\t" << m_JointFeedback.t1[2] << "\t" <<
                             m_JointFeedback.f2[0] << "\t" << m_JointFeedback.f2[1] << "\t" << m_JointFeedback.f2[2] << "\t" <<
                             m_JointFeedback.t2[0] << "\t" << m_JointFeedback.t2[1] << "\t" << m_JointFeedback.t2[2] << "\t" <<
                             "\n";
        }
        else
        {
            if (m_stressCalculationType == beam)
            {
                dVector3 r;
                dBodyGetRelPointPos (this->GetBody1()->GetBodyID(), m_StressOrigin.x, m_StressOrigin.y, m_StressOrigin.z, r);
                *m_DumpStream << gSimulation->GetTime() << "\t" << r[0] << "\t" << r[1] << "\t" << r[2] << "\t" <<
                                 m_JointFeedback.f1[0] << "\t" << m_JointFeedback.f1[1] << "\t" << m_JointFeedback.f1[2] << "\t" <<
                                 m_JointFeedback.t1[0] << "\t" << m_JointFeedback.t1[1] << "\t" << m_JointFeedback.t1[2] << "\t" <<
                                 m_forceStressCoords.x << "\t" << m_forceStressCoords.y << "\t" << m_forceStressCoords.z << "\t" <<
                                 m_torqueStressCoords.x << "\t" << m_torqueStressCoords.y << "\t" << m_torqueStressCoords.z << "\t" <<
                                 m_minStress << "\t" << m_maxStress << "\t" << m_timeAveragedMinStress << "\t" << m_timeAveragedMaxStress << "\t" <<
                                 "\n";
            }
            else if (m_stressCalculationType == spring)
            {
                dVector3 r;
                dBodyGetRelPointPos (this->GetBody1()->GetBodyID(), m_StressOrigin.x, m_StressOrigin.y, m_StressOrigin.z, r);
                *m_DumpStream << gSimulation->GetTime() << "\t" << r[0] << "\t" << r[1] << "\t" << r[2] << "\t" <<
                                 m_JointFeedback.f1[0] << "\t" << m_JointFeedback.f1[1] << "\t" << m_JointFeedback.f1[2] << "\t" <<
                                 m_JointFeedback.t1[0] << "\t" << m_JointFeedback.t1[1] << "\t" << m_JointFeedback.t1[2] << "\t" <<
                                 m_forceStressCoords.x << "\t" << m_forceStressCoords.y << "\t" << m_forceStressCoords.z << "\t" <<
                                 m_torqueAxis.x << "\t" << m_torqueAxis.y << "\t" << m_torqueAxis.z << "\t" <<
                                 m_torqueScalar << "\t" << m_minStress << "\t" << m_maxStress << "\t" << m_timeAveragedMinStress << "\t" << m_timeAveragedMaxStress << "\t" <<
                                 "\n";
            }

            // output the time averaged stress bitmap but only every m_stressWindow counts
            int ix, iy, i;
            double *stressPtr = m_stress;
            for (i = 0; i < m_nActivePixels; i++)
            {
                m_stressCompleteTotal[i] -= m_stressCompleteList[i + m_stressListIndex * m_nActivePixels];
                m_stressCompleteList[i + m_stressListIndex * m_nActivePixels] = *stressPtr;
                m_stressCompleteTotal[i] += *stressPtr;
                stressPtr++;
            }

            if (m_dumpCount % m_stressWindow == 0 && m_dumpCount > 0)
            {

                TIFFWrite tiff;
                tiff.initialiseImage(m_nx, m_ny, 72, 72, 3);
                unsigned char *row = new unsigned char[m_nx * 3];
                unsigned char *rowPtr;
                double v;
                int idx;
                double lowRange = DBL_MAX;
                double highRange = -DBL_MAX;
                unsigned char *stiffnessPtr = m_stiffness;
                if (m_lowRange != m_highRange)
                {
                    lowRange = m_lowRange;
                    highRange = m_highRange;
                }
                else
                {
                    stressPtr = m_stressCompleteTotal;
                    for (i = 0; i < m_nActivePixels; i++)
                    {
                        v = ((*stressPtr) / m_stressWindow);
                        if (v > highRange) highRange = v;
                        if (v < lowRange) lowRange = v;
                        stressPtr++;
                    }
                }
                stressPtr = m_stressCompleteTotal;
                for (iy = 0; iy < m_ny; iy++)
                {
                    rowPtr = row;
                    for (ix = 0; ix < m_nx; ix++)
                    {
                        if (*stiffnessPtr)
                        {
                            v = (((*stressPtr) / m_stressWindow) - lowRange) / (highRange - lowRange);
                            idx = (int)(256.0 * v);
                            if (idx < 0) idx = 0;
                            else if (idx > 255) idx = 255;
                            *rowPtr++ = m_colourMap[idx];
                            *rowPtr++ = m_colourMap[idx + 256];
                            *rowPtr++ = m_colourMap[idx + 512];
                            stressPtr++;
                        }
                        else
                        {
                            *rowPtr++ = 0;
                            *rowPtr++ = 0;
                            *rowPtr++ = 0;
                        }
                        stiffnessPtr++;
                    }
                    tiff.copyRow(iy, row);
                }
                char *pathname = new char[m_Name.length() + 32];
                sprintf(pathname, "%s%04d.tif", m_Name.c_str(), m_dumpCount / m_stressWindow);
                tiff.writeToFile(pathname);
                delete [] pathname;
                delete [] row;
            }

        }

        m_dumpCount++;
    }
}
#endif

#ifdef USE_OPENGL
void FixedJoint::Draw()
{
#ifdef EXPERIMENTAL
    if (m_Visible == false) return;

    if (m_stiffness == 0 || m_texture == 0) return;

    if (m_LastDisplayTime != gSimulation->GetTime())
    {
        m_LastDisplayTime = gSimulation->GetTime();

        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        unsigned char *texturePtr;
        int ix, iy;
        unsigned char *stiffnessPtr;
        if (gSimulation->GetTime() <= 0) // set texture from stiffness bitmap (0 or 1)
        {
            texturePtr = m_texture;
            stiffnessPtr = m_stiffness;
            for (iy = 0; iy < m_ny; iy++)
            {
                for (ix = 0; ix < m_nx; ix++)
                {
                    if (*stiffnessPtr)
                    {
                        *texturePtr++ = 255;
                        *texturePtr++ = 255;
                        *texturePtr++ = 255;
                        *texturePtr++ = 255;
                    }
                    else
                    {
                        texturePtr += 4;
                    }
                    stiffnessPtr++;
                }
            }
        }
        else
        {
            if (g_UseOpenCL == false)
            {
                texturePtr = m_texture;
                stiffnessPtr = m_stiffness;

                double *stressPtr = m_stress;
                double v;
                int idx;
                for (iy = 0; iy < m_ny; iy++)
                {
                    for (ix = 0; ix < m_nx; ix++)
                    {
                        if (*stiffnessPtr)
                        {
                            if (m_lowRange != m_highRange)
                            {
                                v = (*stressPtr - m_lowRange) / (m_highRange - m_lowRange);
                            }
                            else
                            {
                                if (m_minStress != m_maxStress)
                                    v = (*stressPtr - m_minStress) / (m_maxStress - m_minStress);
                                else
                                    v = 0;
                            }
                            idx = (int)(255.0 * v);
                            *texturePtr++ = m_colourMap[idx];
                            *texturePtr++ = m_colourMap[idx + 256];
                            *texturePtr++ = m_colourMap[idx + 512];
                            *texturePtr++ = 255;
                            stressPtr++;
                        }
                        else
                        {
                            texturePtr += 4;
                        }
                        stiffnessPtr++;
                    }
                }
            }
            else
            {
#ifdef USE_OPENCL_BROKEN
                cl_int resultCL;
                float low, high;
                try
                {
                    if (m_lowRange != m_highRange)
                    {
                        low = m_lowRange;
                        high = m_highRange;
                    }
                    else
                    {
                        if (m_minStress != m_maxStress)
                        {
                            low = m_minStress;
                            high = m_maxStress;
                        }
                        else
                        {
                            low = high = 0;
                        }
                    }
                    // set the kernel arguments
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 0, sizeof(cl_mem), (void *)&m_stiffnessBuffer);
                    if (resultCL != CL_SUCCESS) throw __LINE__;
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 1, sizeof(cl_mem), (void *)&m_stressBuffer);
                    if (resultCL != CL_SUCCESS) throw __LINE__;
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 2, sizeof(cl_mem), (void *)&m_colourMapBuffer);
                    if (resultCL != CL_SUCCESS) throw __LINE__;
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 3, sizeof(cl_mem), (void *)&m_textureBuffer);
                    if (resultCL != CL_SUCCESS) throw __LINE__;
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 4, sizeof(float), (void *)&low);
                    if (resultCL != CL_SUCCESS) throw __LINE__;
                    resultCL = clSetKernelArg(oclHandles.kernel[1], 5, sizeof(float), (void *)&high);
                    if (resultCL != CL_SUCCESS) throw __LINE__;

                    // and do the kernal
                    cl_event events[1];
                    const int WORK_DIM = 2;
                    size_t globalNDWorkSize[WORK_DIM];  //Total number of work items
                    size_t localNDWorkSize[WORK_DIM];   //Work items in each work-group
                    globalNDWorkSize[0] = m_nx;
                    globalNDWorkSize[1] = m_ny;
                    if (OpenCLRoutines::GetMaxWorkgroupSize() < 256)
                    {
                        localNDWorkSize[0] = 1;
                        localNDWorkSize[1] = 1;
                    }
                    else
                    {
                        localNDWorkSize[0] = 16;
                        localNDWorkSize[1] = 16;
                    }

                    resultCL = clEnqueueNDRangeKernel(oclHandles.queue,
                                                      oclHandles.kernel[1],
                                                      WORK_DIM,
                                                      NULL,
                                                      globalNDWorkSize,
                                                      localNDWorkSize,
                                                      0, NULL,
                                                      &events[0]);
                    if (resultCL != CL_SUCCESS) throw __LINE__;

                    /* wait for the kernel call to finish execution */
                    /* (note clFinish might be an alternative ) */
                    resultCL = clWaitForEvents(1, &events[0]);
                    if (resultCL != CL_SUCCESS) throw __LINE__;

                    resultCL = clEnqueueReadBuffer(oclHandles.queue,            // cl_command_queue command_queue
                                                   m_textureBuffer,             // cl_mem buffer
                                                   CL_TRUE,                     // cl_bool blocking_read
                                                   0,                           // size_t offset
                                                   4 * m_nx * m_ny,             // size_t cb
                                                   m_texture,                   // void *ptr
                                                   0,                           // cl_uint num_events_in_wait_list
                                                   NULL,                        // const cl_event *event_wait_list
                                                   NULL);                       // cl_event *event
                    if (resultCL != CL_SUCCESS) throw __LINE__;

                }

                catch (int e)
                {
                    char *errStr = OpenCLRoutines::GetErrorString(resultCL);
                    std::cerr << "FixedJoint::CalculateStress error in line " << e << "\n" << errStr << "\n";
                    free(errStr);
                }
#endif
            }
        }

        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, m_nx, m_ny, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_texture); // glTexSubmImage2D may be quicker

    }

    GLfloat xmin = -m_xOrigin;
    GLfloat ymin = -m_yOrigin;
    GLfloat xmax = xmin + m_nx * m_dx;
    GLfloat ymax = ymin + m_ny * m_dy;

    // calculate the quaternion that rotates from stress coordinates to world coordinates
    const double *q = dBodyGetQuaternion(this->GetBody1()->GetBodyID());
    pgd::Quaternion qBody(q[0], q[1], q[2], q[3]);
    pgd::Quaternion stressToWorldQuaternion =  qBody * m_StressOrientation;
    // pgd::Quaternion worldToStressQuaternion = m_StressOrientation * qBody;

    // calculate the world corrdinates of the to stress position
    dVector3 worldStressOrigin;
    dBodyGetRelPointPos (this->GetBody1()->GetBodyID(), m_StressOrigin.x, m_StressOrigin.y, m_StressOrigin.z, worldStressOrigin);

    glPushMatrix();

    glTranslatef(worldStressOrigin[0], worldStressOrigin[1], worldStressOrigin[2]);
    glRotatef(pgd::RadiansToDegrees(2*acos(stressToWorldQuaternion.n)), stressToWorldQuaternion.v.x, stressToWorldQuaternion.v.y, stressToWorldQuaternion.v.z);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBegin (GL_QUADS);
    glTexCoord2f (0.0, 0.0);
    glVertex3f (xmin, ymin, 0.0);
    glTexCoord2f (1.0, 0.0);
    glVertex3f (xmax, ymin, 0.0);
    glTexCoord2f (1.0, 1.0);
    glVertex3f (xmax, ymax, 0.0);
    glTexCoord2f (0.0, 1.0);
    glVertex3f (xmin, ymax, 0.0);
    glEnd ();

    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    if (gAxisFlag)
        GLUtils::DrawAxes(m_AxisSize[0], m_AxisSize[1], m_AxisSize[2]);

    glPopMatrix();

#endif
}

#endif
