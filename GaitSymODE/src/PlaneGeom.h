/*
 *  PlaneGeom.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 13/09/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef PlaneGeom_h
#define PlaneGeom_h

#include "Geom.h"

#include <ode/ode.h>

class PlaneGeom: public Geom
{
public:

    PlaneGeom(dSpaceID space, double a, double b, double c, double d);
    ~PlaneGeom();

#ifdef USE_OPENGL
    void SetTrackPatch(double trackPatchStartX, double trackPatchStartY, double trackPatchEndX, double trackPatchEndY, double trackPatchResolutionX, double trackPatchResolutionY);
    void SetTrackSensitivity(double lowRange, double highRange) { m_lowRange = lowRange; m_highRange = highRange; }
    void SetCheckerboardLow(double checkerboardLow) { m_checkerboardLow = checkerboardLow; }
    void SetCheckerboardHigh(double checkerboardHigh) { m_checkerboardHigh = checkerboardHigh; }
    void SetTrackDrawThreshold(double trackDrawThreshold) { m_trackDrawThreshold = trackDrawThreshold; }
    void AddImpulse(double x, double y, double z, double fx, double fy, double fz, double time);
    virtual void Draw();
#endif

protected:

#ifdef USE_OPENGL
    GLuint m_textureID;
    double m_lastDisplayTime;
    unsigned char *m_texture;
    int m_nx;
    int m_ny;
    double m_trackPatchStartX;
    double m_trackPatchEndX;
    double m_trackPatchResolutionX;
    double m_trackPatchStartY;
    double m_trackPatchEndY;
    double m_trackPatchResolutionY;
    double *m_trackDepth;
    double m_lowRange;
    double m_highRange;
    unsigned char m_colourMap[256][4]; // RGBA colour map
    pgd::Vector m_planeOrigin;
    pgd::Quaternion m_planeOrientation;
    double m_checkerboardLow;
    double m_checkerboardHigh;
    double m_trackDrawThreshold;
#endif

};


#endif
