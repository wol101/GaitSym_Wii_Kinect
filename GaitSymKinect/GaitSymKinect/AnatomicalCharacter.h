/*
 *  AnatomicalCharacter.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 21/12/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef ANATOMICALCHARACTER_H
#define ANATOMICALCHARACTER_H

#include "PGDMath.h"

struct KinectCharacter;

enum AnatomicalJointID
{
    AnatomicalLeftHip = 0,
    AnatomicalLeftKnee = 1,
    AnatomicalLeftAnkle = 2,
    AnatomicalRightHip = 3,
    AnatomicalRightKnee = 4,
    AnatomicalRightAnkle = 5,
    AnatomicalLeftShoulder = 6,
    AnatomicalLeftElbow = 7,
    AnatomicalLeftWrist = 8,
    AnatomicalRightShoulder = 9,
    AnatomicalRightElbow = 10,
    AnatomicalRightWrist = 11,
    AnatomicalJointIDCount = 12
};

enum AnatomicalBodyID
{
    AnatomicalTrunk = 0,
    AnatomicalLeftThigh = 1,
    AnatomicalLeftShank = 2,
    AnatomicalLeftFoot = 3,
    AnatomicalRightThigh = 4,
    AnatomicalRightShank = 5,
    AnatomicalRightFoot = 6,
    AnatomicalLeftArm = 7,
    AnatomicalLeftForearm = 8,
    AnatomicalLeftHand = 9,
    AnatomicalRightArm = 10,
    AnatomicalRightForearm = 11,
    AnatomicalRightHand = 12,
    AnatomicalBodyIDCount = 13
};

struct AnatomicalBasis
{
    pgd::Vector X;
    pgd::Vector Y;
    pgd::Vector Z;
};

struct AnatomicalMovement
{
    AnatomicalMovement() { extension = 0; abduction = 0; rotation = 0; }
    double extension;
    double abduction;
    double rotation;
};

inline const char* ToString(AnatomicalJointID v)
{
    switch (v)
    {
    case AnatomicalLeftHip: return "AnatomicalLeftHip";
    case AnatomicalLeftKnee: return "AnatomicalLeftKnee";
    case AnatomicalLeftAnkle: return "AnatomicalLeftAnkle";
    case AnatomicalRightHip: return "AnatomicalRightHip";
    case AnatomicalRightKnee: return "AnatomicalRightKnee";
    case AnatomicalRightAnkle: return "AnatomicalRightAnkle";
    case AnatomicalLeftShoulder: return "AnatomicalLeftShoulder";
    case AnatomicalLeftElbow: return "AnatomicalLeftElbow";
    case AnatomicalLeftWrist: return "AnatomicalLeftWrist";
    case AnatomicalRightShoulder: return "AnatomicalRightShoulder";
    case AnatomicalRightElbow: return "AnatomicalRightElbow";
    case AnatomicalRightWrist: return "AnatomicalRightWrist";
    default: return "[Unknown Joint]";
    }
}

inline const char* ToString(AnatomicalBodyID v)
{
    switch (v)
    {
    case AnatomicalTrunk: return "AnatomicalTrunk";
    case AnatomicalLeftThigh: return "AnatomicalLeftThigh";
    case AnatomicalLeftShank: return "AnatomicalLeftShank";
    case AnatomicalLeftFoot: return "AnatomicalLeftFoot";
    case AnatomicalRightThigh: return "AnatomicalRightThigh";
    case AnatomicalRightShank: return "AnatomicalRightShank";
    case AnatomicalRightFoot: return "AnatomicalRightFoot";
    case AnatomicalLeftArm: return "AnatomicalLeftArm";
    case AnatomicalLeftForearm: return "AnatomicalLeftForearm";
    case AnatomicalLeftHand: return "AnatomicalLeftHand";
    case AnatomicalRightArm: return "AnatomicalRightArm";
    case AnatomicalRightForearm: return "AnatomicalRightForearm";
    case AnatomicalRightHand: return "AnatomicalRightHand";
    default: return "[Unknown Body]";
    }
}

class AnatomicalCharacter
{
public:
    AnatomicalCharacter();
    ~AnatomicalCharacter();

    void UpdateAnatomicalJoints();
    double GetAngle(AnatomicalJointID jointID, int index);

    KinectCharacter *kinectCharacter() const;
    void setKinectCharacter(KinectCharacter *kinectCharacter);

protected:
    KinectCharacter *m_kinectCharacter;
    AnatomicalBasis *m_basisList;
    AnatomicalMovement *m_movementList;
};

#endif // ANATOMICALCHARACTER_H
