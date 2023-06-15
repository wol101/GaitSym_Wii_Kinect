/*
 *  Strap.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 29/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef Strap_h
#define Strap_h

#include "NamedObject.h"
#include "Simulation.h"
#include <vector>

class Body;
class FacetedObject;

struct PointForce
{
    Body *body;         // this is the body the force is applied to
    dVector3 point;     // this is the position of action in world coordinates
    dVector3 vector;    // this is the direction of action (magnitude acts as a scaling factor)
};

class Strap: public NamedObject
{
public:

    Strap();
    virtual ~Strap();

    double GetLength() { return m_Length; };
    double GetVelocity() { return m_Velocity; };

    void SetTension(double tension) { m_Tension = tension; };
    double GetTension() { return m_Tension; };

    std::vector<PointForce *> *GetPointForceList() { return &m_PointForceList; };

    virtual void Calculate(double deltaT) = 0;

    virtual int SanityCheck(Strap *otherStrap, AxisType axis, const std::string &sanityCheckLeft, const std::string &sanityCheckRight) = 0;

    virtual void Dump();

#ifdef USE_OPENGL
    virtual void Draw() = 0;
    void SetRadius(GLfloat strapSize) { m_Radius = strapSize; };
    void SetForceColour(Colour &colour) { m_ForceColour = colour; };
    void SetForceRadius(GLfloat forceSize) { m_ForceRadius = forceSize; };
    void SetForceScale(GLfloat forceScale) { m_ForceScale = forceScale; };
    void SetLastDrawTime(double lastDrawTime) { m_LastDrawTime = lastDrawTime; }
    GLfloat GetRadius() { return m_Radius; };
    const Colour *GetForceColour() { return &m_ForceColour; };
    GLfloat GetForceRadius() { return m_ForceRadius; };
    GLfloat GetForceScale() { return m_ForceScale; };
#endif

protected:

    double m_Length;
    double m_LastLength;
    double m_Velocity;
    double m_Tension;

    std::vector<PointForce *> m_PointForceList;

#ifdef USE_OPENGL
    Colour m_ForceColour;
    GLfloat m_ForceRadius;
    GLfloat m_ForceScale;
    GLfloat m_Radius;

    std::vector<FacetedObject *> m_DrawList;
    double m_LastDrawTime;
#endif
};

#endif

