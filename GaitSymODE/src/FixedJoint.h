/*
 *  FixedJoint.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 20/09/2008.
 *  Copyright 2008 Bill Sellers. All rights reserved.
 *
 */

#ifndef FixedJoint_h
#define FixedJoint_h

#ifdef USE_OPENCL
#include <cl.h>
#endif

#include "Joint.h"
#include "PGDMath.h"

#ifdef EXPERIMENTAL
enum StressCalculationType
{
    none = 0, beam, spring
};
#endif

class FixedJoint: public Joint
{
    public:

    FixedJoint(dWorldID worldID);
    ~FixedJoint();

    void SetFixed();

#ifdef EXPERIMENTAL
    void SetCrossSection(unsigned char *stiffness, int nx, int ny, double dx, double dy);
    void SetStressOrigin(double x, double y, double z);
    void SetStressOrigin(const char *buf);
    void SetStressOrientation(double q0, double q1, double q2, double q3);
    void SetStressOrientation(const char *buf);
    void SetStressCalculationType(StressCalculationType type) { m_stressCalculationType = type; }

    pgd::Vector GetStressOrigin() { return m_StressOrigin; }
    pgd::Quaternion GetStressOrientation() { return m_StressOrientation; }

    double GetMaxStress() { return m_maxStress; }
    double GetMinStress() { return m_minStress; }

    void SetStressLimit(double stressLimit, int stressWindow);
    bool CheckStressAbort();
    double GetTimeAveragedMaxStress() { return m_timeAveragedMaxStress; }
    double GetTimeAveragedMinStress() { return m_timeAveragedMinStress; }

    double *GetStress() { return m_stress; }

    void SetDisplayRange(double low, double high) { m_lowRange = low; m_highRange = high; }


    virtual void Update();
    virtual void Dump();

#ifdef USE_OPENGL
    void SetTextureID(int textureID) { m_textureID = textureID; }
#endif

#endif

#ifdef USE_OPENGL
    virtual void Draw();
#endif


protected:

#ifdef EXPERIMENTAL
    void CalculateStress();

    // these are used for the stress/strain calculations
    unsigned char *m_stiffness;
    double *m_stress;
    double *m_xDistances;
    double *m_yDistances;
    int m_nx;
    int m_ny;
    int m_nActivePixels;
    double m_dx;
    double m_dy;
    double m_Ix;
    double m_Iy;
    double m_Ixy;
    double m_area;
    double m_xOrigin;
    double m_yOrigin;
    double m_minStress;
    double m_maxStress;
    StressCalculationType m_stressCalculationType;

    pgd::Vector m_StressOrigin;
    pgd::Quaternion m_StressOrientation;
    pgd::Vector m_torqueStressCoords;
    pgd::Vector m_forceStressCoords;
    pgd::Vector m_torqueAxis;
    double m_torqueScalar;

    double m_stressLimit;
    double *m_stressListMin;
    double *m_stressListMax;
    int m_stressListIndex;
    int m_stressWindow;
    double m_timeAveragedMaxStress;
    double m_timeAverageTotalMaxStress;
    double m_timeAveragedMinStress;
    double m_timeAverageTotalMinStress;

    pgd::Vector *m_vectorList;

    int m_dumpCount;
    double *m_stressCompleteList;
    double *m_stressCompleteTotal;

    double m_lowRange;
    double m_highRange;
    unsigned char *m_colourMap;

#ifdef USE_OPENGL
    GLuint m_textureID;
    double m_LastDisplayTime;
    unsigned char *m_texture;
#endif

#ifdef USE_OPENCL
    cl_mem m_stiffnessBuffer;
    cl_mem m_xDistanceBuffer;
    cl_mem m_yDistanceBuffer;
    cl_mem m_stressBuffer;
    cl_mem m_minStressBuffer;
    cl_mem m_maxStressBuffer;
    cl_mem m_textureBuffer;
    cl_mem m_colourMapBuffer;
#endif

#endif

};



#endif
