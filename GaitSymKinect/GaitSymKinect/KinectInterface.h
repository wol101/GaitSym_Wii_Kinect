/*
 *  KinectInterface.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef KINECTINTERFACE_H
#define KINECTINTERFACE_H

#include <stdint.h>

#include "PGDMath.h"

struct IBody;
struct IKinectSensor;
struct ICoordinateMapper;
struct IBodyFrameReader;
struct IDepthFrameReader;

typedef struct _CameraSpacePoint CameraSpacePoint;
typedef struct tagRGBQUAD RGBQUAD;

enum KinectJointID
{
    SpineBase = 0,
    SpineMid = 1,
    Neck = 2,
    Head = 3,
    ShoulderLeft = 4,
    ElbowLeft = 5,
    WristLeft = 6,
    HandLeft = 7,
    ShoulderRight = 8,
    ElbowRight = 9,
    WristRight = 10,
    HandRight = 11,
    HipLeft = 12,
    KneeLeft = 13,
    AnkleLeft = 14,
    FootLeft = 15,
    HipRight = 16,
    KneeRight = 17,
    AnkleRight = 18,
    FootRight = 19,
    SpineShoulder = 20,
    HandTipLeft = 21,
    ThumbLeft = 22,
    HandTipRight = 23,
    ThumbRight = 24,
    KinectJointIDCount = 25
};

inline const char* ToString(KinectJointID v)
{
    switch (v)
    {
    case SpineBase: return "SpineBase";
    case SpineMid: return "SpineMid";
    case Neck: return "Neck";
    case Head: return "Head";
    case ShoulderLeft: return "ShoulderLeft";
    case ElbowLeft: return "ElbowLeft";
    case WristLeft: return "WristLeft";
    case HandLeft: return "HandLeft";
    case ShoulderRight: return "ShoulderRight";
    case ElbowRight: return "ElbowRight";
    case WristRight: return "WristRight";
    case HandRight: return "HandRight";
    case HipLeft: return "HipLeft";
    case KneeLeft: return "KneeLeft";
    case AnkleLeft: return "AnkleLeft";
    case FootLeft: return "FootLeft";
    case HipRight: return "HipRight";
    case KneeRight: return "KneeRight";
    case AnkleRight: return "AnkleRight";
    case FootRight: return "FootRight";
    case SpineShoulder: return "SpineShoulder";
    case HandTipLeft: return "HandTipLeft";
    case ThumbLeft: return "ThumbLeft";
    case HandTipRight: return "HandTipRight";
    case ThumbRight: return "ThumbRight";
    default: return "[Unknown Joint]";
    }
}

enum KinectBodyID
{
    // Torso
    Skull = 0, // JointType_Head, JointType_Neck
    Cervical = 1, // JointType_Neck, JointType_SpineShoulder
    Thoracic = 2, // JointType_SpineShoulder, JointType_SpineMid
    Lumbar = 3, // JointType_SpineMid, JointType_SpineBase
    RightScapula = 4, // JointType_SpineShoulder, JointType_ShoulderRight
    LeftScapula = 5, // JointType_SpineShoulder, JointType_ShoulderLeft
    RightPelvis = 6, // JointType_SpineBase, JointType_HipRight
    LeftPelvis = 7, // JointType_SpineBase, JointType_HipLeft

    // Right Arm
    RightArm = 8, // JointType_ShoulderRight, JointType_ElbowRight
    RightForearm = 9, // JointType_ElbowRight, JointType_WristRight
    RightPalm = 10, // JointType_WristRight, JointType_HandRight
    RightIndex = 11, // JointType_HandRight, JointType_HandTipRight
    RightThumb = 12, // JointType_WristRight, JointType_ThumbRight

    // Left Arm
    LeftArm = 13, // JointType_ShoulderLeft, JointType_ElbowLeft
    LeftForearm = 14, // JointType_ElbowLeft, JointType_WristLeft
    LeftPalm = 15, // JointType_WristLeft, JointType_HandLeft
    LeftIndex = 16, // JointType_HandLeft, JointType_HandTipLeft
    LeftThumb = 17, // JointType_WristLeft, JointType_ThumbLeft

    // Right Leg
    RightThigh = 18, // JointType_HipRight, JointType_KneeRight
    RightShank = 19, // JointType_KneeRight, JointType_AnkleRight
    RightFoot = 20, // JointType_AnkleRight, JointType_FootRight

    // Left Leg
    LeftThigh = 21, // JointType_HipLeft, JointType_KneeLeft
    LeftShank = 22, // JointType_KneeLeft, JointType_AnkleLeft
    LeftFoot = 23, // JointType_AnkleLeft, JointType_FootLeft

    KinectBodyIDCount = 24
};

enum KinectTrackingState
{
    NotTracked = 0,
    Inferred = 1,
    Tracked = 2
};

struct KinectJoint
{
    KinectTrackingState state;
    pgd::Vector cameraSpaceLocation;
    pgd::Vector depthSpaceLocation;
    pgd::Quaternion jointOrinetation;
};

struct KinectBody
{
    KinectJointID jointID1;
    KinectJointID jointID2;
};

struct KinectCharacter
{
    KinectJoint joints[KinectJointIDCount];
    KinectBody bodies[KinectBodyIDCount];
    bool tracked;
};

class KinectInterface
{
public:
    KinectInterface();
    ~KinectInterface();

    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;

    void UpdateKinect();
    void UpdateBodies();
    const KinectJoint *GetJointData(int characterNum, KinectJointID id) { return &m_characterList[characterNum].joints[id]; }
    const KinectBody *GetBodyData(int characterNum, KinectBodyID id) { return &m_characterList[characterNum].bodies[id]; }
    bool IsTracking(int characterNum) { return m_characterList[characterNum].tracked; }
    KinectCharacter *GetKinectCharacter(int characterNum) { return &m_characterList[characterNum]; }
    int GetMaxCharacters();
    const RGBQUAD *GetDepthRGBX() { return m_pDepthRGBX; }

private:
    long InitializeDefaultSensor();
    void ProcessBody(int64_t nTime, int nBodyCount, IBody** ppBodies);
    void InitialiseBodies();
    void ProcessDepth(int64_t nTime, const uint16_t *buffer, int nWidth, int nHeight, uint16_t nMinDepth, uint16_t nMaxDepth);

    // Current Kinect
    IKinectSensor *m_pKinectSensor;
    ICoordinateMapper *m_pCoordinateMapper;

    // Body reader
    IBodyFrameReader *m_pBodyFrameReader;

    // Depth reader
    IDepthFrameReader *m_pDepthFrameReader;

    long long m_nStartTime;

    KinectCharacter *m_characterList;
    int m_nChactacters;

    // range data image
    RGBQUAD *m_pDepthRGBX;
    CameraSpacePoint *m_cameraSpacePoints;
};

#endif // KINECTINTERFACE_H
