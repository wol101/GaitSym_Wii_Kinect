/*
 *  KinectConnector.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 16/10/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef KINECTCONNECTOR_H
#define KINECTCONNECTOR_H

#include "AnatomicalCharacter.h"

#include <QString>

#include <map>
#include <string>

class FixedDriver;

struct KinectConnectorParameters
{
    KinectConnectorParameters()
    {
        fixedDriver = 0;
        gain = 0;
        neutralAngle = 0;
        lowerBound = -1;
        upperBound = 1;
    }
    FixedDriver *fixedDriver;
    double gain;
    double neutralAngle;
    double lowerBound;
    double upperBound;
};

class KinectConnectorJoint
{
public:
    KinectConnectorJoint();
    ~KinectConnectorJoint();

    void SetJointAngle(double jointAngle) { m_jointAngle = jointAngle; }
    void SetDrivablesAbsolute(double newDrivableValue);
    void UpdateDrivables();
    KinectConnectorParameters *GetKinectConnectorParameter(const std::string driverName);
    void SetKinectConnectorParameter(KinectConnectorParameters *kinectParameters);
    void DeleteKinectConnectorParameter(const std::string driverName);

protected:
    std::map<std::string, KinectConnectorParameters *> m_parameterList;
    double m_jointAngle;
};

class KinectConnector
{
public:
    KinectConnector();
    ~KinectConnector();

    void SetJointAngle(AnatomicalJointID kinectJointId, int eulerAngleIndex, double angle);
    void SetDrivablesAbsolute(AnatomicalJointID kinectJointId, int eulerAngleIndex, double value);
    void UpdateDrivables(AnatomicalJointID kinectJointId, int eulerAngleIndex);

    KinectConnectorJoint *GetKinectConnectorJoint(AnatomicalJointID kinectJointId, int eulerAngleIndex);

    int Write(QString filename);
    int Read(QString filename);

protected:

    KinectConnectorJoint *m_KinectConnectorJoints;
};

#endif // KINECTCONNECTOR_H
