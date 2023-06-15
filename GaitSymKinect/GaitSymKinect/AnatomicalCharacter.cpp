/*
 *  AnatomicalCharacter.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 21/12/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "AnatomicalCharacter.h"

#include "KinectInterface.h"

#include <iostream>

AnatomicalCharacter::AnatomicalCharacter()
{
    m_kinectCharacter = 0;
    m_basisList = new AnatomicalBasis[AnatomicalBodyIDCount];
    m_movementList = new AnatomicalMovement[AnatomicalJointIDCount];
}

AnatomicalCharacter::~AnatomicalCharacter()
{
    delete [] m_basisList;
    delete [] m_movementList;
}

/*
 * Need to fit a figure to the available joints to calculate major joint movements.
 * Start by defining the trunk basis for reference. Fit a line to the spine points
 * to define z, then  use the positions of the hip joints to define the y. Then the
 * x can be obtained by cross product. Actually down the spine might define x best,
 * then right to left hip for y, which would leave z forward. This would be consistent
 * with the other bodies in the model.
 *
 * For the thigh, define x by knee and hip. Then cross product by the thigh and shank
 * to define y, and cross product x and y to define z. If the angle between the thigh
 * and shank is too low eg less than 10 degrees, use the cross product of the thigh
 * and foot instead. Use this y value for the foot too, with the x defined by the ankle
 * and toe and z by cross product.
 *
 * For the arm define x by the elbow and shoulder. If the forearm-arm angle is high
 * enough define the y by cross product and the z by cross product again, but if the
 * forearm-arm angle is too low to reliably generate the y vector then just use the
 * previous good one.
 *
 * For the forearm use the elbow to wrist vector for the x and the same y as the arm,
 * and calculate the z by cross product. For the hand assume any wrist movement is
 * flexion but require quite a large angle to confirm. The x is wrist to fingertip,
 * the y is cross product of forearm x and hand x vectors, and the z is the cross
 * product of the hand x and hand y vectors. The wrist y vector is likely to be quite
 * unreliable and might need to default to the elbow y, or the last good value. It
 * might also need something to check that it doesn't move by a huge amount.
 *
 * Once the basis is defined the movements can be described by projecting the basis
 * onto the axes of the parent basis. Projection is just done with the dot product,
 * and atan2 will give a fully resolved angle - useful for the shoulder. The only
 * slightly difficult projection is the one required to get the axial rotation. This
 * moves with the limb so needs a vector to define zero. For the shoulder and hip this
 * is probably the body Y axis, but will need to switch to the body x axis when the
 * limb is fully abducted or we will get a singularity (not likely for the hip, but
 * very likely for the shoulder).
 *
 * Generally the upper limb is the troublesome one.
 */

void AnatomicalCharacter::UpdateAnatomicalJoints()
{
    // these are the thresholds in degrees used to orient the axial rotations of the limbs
    const double k_minKneeAngle = 10;
    const double k_minFootShankAngle = 10;
    const double k_minElbowAngle = 10;
    const double k_minWristAngle = 30;

    pgd::Vector x, y, z, v;
    double theta;

    if (m_kinectCharacter == 0)
    {
        std::cerr << "m_kinectCharacter undefined in AnatomicalCharacter::UpdateAnatomicalJoints\n";
        return;
    }

    // start with the X axis of the body
    x = m_kinectCharacter->joints[SpineBase].cameraSpaceLocation - m_kinectCharacter->joints[SpineMid].cameraSpaceLocation;
    x.Normalize();
    // then the y axis
    y = m_kinectCharacter->joints[HipLeft].cameraSpaceLocation - m_kinectCharacter->joints[HipRight].cameraSpaceLocation;
    y.Normalize();
    // and the z by cross product
    z = x ^ y;
    m_basisList[AnatomicalTrunk].X = x;
    m_basisList[AnatomicalTrunk].Y = y;
    m_basisList[AnatomicalTrunk].Z = z;

    // now the X axis of the left thigh
    x = m_kinectCharacter->joints[KneeLeft].cameraSpaceLocation - m_kinectCharacter->joints[HipLeft].cameraSpaceLocation;
    x.Normalize();
    // now check the angle between the shank and the thigh
    v = m_kinectCharacter->joints[AnkleLeft].cameraSpaceLocation - m_kinectCharacter->joints[KneeLeft].cameraSpaceLocation;
    v.Normalize();
    theta = pgd::AngleBetweenNormalised(v, x) * 180/M_PI;
    if (theta >= k_minKneeAngle || theta <= 180 - k_minKneeAngle)
    {
        y = v ^ x;
    }
    else
    {
        // check the angle between the foot and the thigh
        v = m_kinectCharacter->joints[FootLeft].cameraSpaceLocation - m_kinectCharacter->joints[AnkleLeft].cameraSpaceLocation;
        v.Normalize();
        theta = pgd::AngleBetweenNormalised(v, x) * 180/M_PI;
        if (theta >= k_minFootShankAngle || theta <= 180 - k_minFootShankAngle)
        {
            y = v ^ x;
            // need a sanity check on y because if could flip quite easily
            if (pgd::AngleBetweenNormalised(y, m_basisList[AnatomicalLeftThigh].Y) > M_PI / 2) y.Reverse();
        }
        else // this is the fail condition so in general just calculate from the last value of Z
        {
            if (m_basisList[AnatomicalLeftThigh].Z.x == 0 && m_basisList[AnatomicalLeftThigh].Z.y == 0 && m_basisList[AnatomicalLeftThigh].Z.z == 0) m_basisList[AnatomicalLeftThigh].Z = m_basisList[AnatomicalTrunk].Z; // this means this basis has never been set
            y = m_basisList[AnatomicalLeftThigh].Z ^ x;
        }
    }
    // and the z by cross product
    z = x ^ y;
    m_basisList[AnatomicalLeftThigh].X = x;
    m_basisList[AnatomicalLeftThigh].Y = y;
    m_basisList[AnatomicalLeftThigh].Z = z;

    // now the X axis of the left shank
    x = m_kinectCharacter->joints[AnkleLeft].cameraSpaceLocation - m_kinectCharacter->joints[KneeLeft].cameraSpaceLocation;
    x.Normalize();
    // the Y axis is the same as the thigh
    y = m_basisList[AnatomicalLeftThigh].Y;
    // and the z by cross product
    z = x ^ y;
    // and recalculate the y to make sure that the basis is orthonormal
    y = z ^ x;
    m_basisList[AnatomicalLeftShank].X = x;
    m_basisList[AnatomicalLeftShank].Y = y;
    m_basisList[AnatomicalLeftShank].Z = z;

    // now the X axis of the left foot
    x = m_kinectCharacter->joints[FootLeft].cameraSpaceLocation - m_kinectCharacter->joints[AnkleLeft].cameraSpaceLocation;
    x.Normalize();
    // the Y axis is the same as the shank
    y = m_basisList[AnatomicalLeftShank].Y;
    // and the z by cross product
    z = x ^ y;
    // and recalculate the y to make sure that the basis is orthonormal
    y = z ^ x;
    m_basisList[AnatomicalLeftFoot].X = x;
    m_basisList[AnatomicalLeftFoot].Y = y;
    m_basisList[AnatomicalLeftFoot].Z = z;

    // now the X axis of the left arm
    x = m_kinectCharacter->joints[ElbowLeft].cameraSpaceLocation - m_kinectCharacter->joints[ShoulderLeft].cameraSpaceLocation;
    x.Normalize();
    // now check the angle between the forearm and the arm
    v = m_kinectCharacter->joints[WristLeft].cameraSpaceLocation - m_kinectCharacter->joints[ElbowLeft].cameraSpaceLocation;
    v.Normalize();
    theta = pgd::AngleBetweenNormalised(v, x) * 180/M_PI;
    if (theta >= k_minElbowAngle || theta <= 180 - k_minElbowAngle)
    {
        y = x ^ v; // the anatomical elbow rotates the other way
    }
    else // this is the fail condition so in general just calculate from the last value of Z
    {
        if (m_basisList[AnatomicalLeftArm].Z.x == 0 && m_basisList[AnatomicalLeftArm].Z.y == 0 && m_basisList[AnatomicalLeftArm].Z.z == 0) m_basisList[AnatomicalLeftArm].Z = m_basisList[AnatomicalTrunk].Z; // this means this basis has never been set
        y = m_basisList[AnatomicalLeftArm].Z ^ x;
    }

    // and the z by cross product
    z = x ^ y;
    m_basisList[AnatomicalLeftArm].X = x;
    m_basisList[AnatomicalLeftArm].Y = y;
    m_basisList[AnatomicalLeftArm].Z = z;

    // now the X axis of the left forearm
    x = m_kinectCharacter->joints[WristLeft].cameraSpaceLocation - m_kinectCharacter->joints[ElbowLeft].cameraSpaceLocation;
    x.Normalize();
    // the Y axis is the same as the arm
    y = m_basisList[AnatomicalLeftArm].Y;
    // and the z by cross product
    z = x ^ y;
    // and recalculate the y to make sure that the basis is orthonormal
    y = z ^ x;
    m_basisList[AnatomicalLeftForearm].X = x;
    m_basisList[AnatomicalLeftForearm].Y = y;
    m_basisList[AnatomicalLeftForearm].Z = z;

    // now the X axis of the left hand
    x = m_kinectCharacter->joints[HandLeft].cameraSpaceLocation - m_kinectCharacter->joints[WristLeft].cameraSpaceLocation;
    x.Normalize();
    // now check the angle between the hand and the forearm
    v = m_kinectCharacter->joints[WristLeft].cameraSpaceLocation - m_kinectCharacter->joints[ElbowLeft].cameraSpaceLocation;
    v.Normalize();
    theta = pgd::AngleBetweenNormalised(v, x) * 180/M_PI;
    if (theta >= k_minWristAngle || theta <= 180 - k_minWristAngle)
    {
        y = x ^ v; // the anatomical elbow rotates the other way
    }
    else // this is the fail condition so in general just use the value from the forearm
    {
        y = m_basisList[AnatomicalLeftForearm].Y;
    }
    // and the z by cross product
    z = x ^ y;
    // and recalculate the y to make sure that the basis is orthonormal
    y = z ^ x;
    m_basisList[AnatomicalLeftHand].X = x;
    m_basisList[AnatomicalLeftHand].Y = y;
    m_basisList[AnatomicalLeftHand].Z = z;

    // now calculate the joint angles
    // hip and shoulder flexion/extension are mapped onto the x,z torso axes
    m_movementList[AnatomicalLeftHip].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftThigh].Z * m_basisList[AnatomicalTrunk].Z, m_basisList[AnatomicalLeftThigh].X * m_basisList[AnatomicalTrunk].X);
    m_movementList[AnatomicalLeftShoulder].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftArm].Z * m_basisList[AnatomicalTrunk].Z, m_basisList[AnatomicalLeftArm].X * m_basisList[AnatomicalTrunk].X);
    // hip and shoulder adduction/abduction are mapped onto the x,y torso axes
    m_movementList[AnatomicalLeftHip].abduction = (180/M_PI) * atan2(m_basisList[AnatomicalLeftThigh].Y * m_basisList[AnatomicalTrunk].Y, m_basisList[AnatomicalLeftThigh].X * m_basisList[AnatomicalTrunk].X);
    m_movementList[AnatomicalLeftShoulder].abduction = (180/M_PI) * atan2(m_basisList[AnatomicalLeftArm].Y * m_basisList[AnatomicalTrunk].Y, m_basisList[AnatomicalLeftArm].X * m_basisList[AnatomicalTrunk].X);
    // knee flexion/extension are mapped onto the thigh x,z axes
    m_movementList[AnatomicalLeftKnee].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftShank].Z * m_basisList[AnatomicalLeftThigh].Z, m_basisList[AnatomicalLeftShank].X * m_basisList[AnatomicalLeftThigh].X);
    // ankle flexion/extension are mapped onto the shank x,z axes
    m_movementList[AnatomicalLeftAnkle].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftFoot].Z * m_basisList[AnatomicalLeftShank].Z, m_basisList[AnatomicalLeftFoot].X * m_basisList[AnatomicalLeftShank].X);
    // elbow flexio/extension are mapped onto the arm x, z axes
    m_movementList[AnatomicalLeftElbow].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftForearm].Z * m_basisList[AnatomicalLeftArm].Z, m_basisList[AnatomicalLeftForearm].X * m_basisList[AnatomicalLeftArm].X);
    // wrist flexio/extension are mapped onto the forearm x, z axes
    m_movementList[AnatomicalLeftWrist].extension = (180/M_PI) * atan2(m_basisList[AnatomicalLeftHand].Z * m_basisList[AnatomicalLeftForearm].Z, m_basisList[AnatomicalLeftHand].X * m_basisList[AnatomicalLeftForearm].X);
}

double AnatomicalCharacter::GetAngle(AnatomicalJointID jointID, int index)
{
    switch (index)
    {
    case 0:
        return m_movementList[jointID].extension;
        break;
    case 1:
        return m_movementList[jointID].abduction;
        break;
    case 2:
        return m_movementList[jointID].rotation;
        break;
    default:
        break;
    }
    return 0;
}


KinectCharacter *AnatomicalCharacter::kinectCharacter() const
{
    return m_kinectCharacter;
}

void AnatomicalCharacter::setKinectCharacter(KinectCharacter *kinectCharacter)
{
    m_kinectCharacter = kinectCharacter;
}


