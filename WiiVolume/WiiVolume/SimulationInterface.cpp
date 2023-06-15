/*
 *  SimulationInterface.cpp
 *  WiiVolume
 *
 *  Created by Bill Sellers on 21/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#include "SimulationInterface.h"
#include "Simulation.h"
#include "FixedDriver.h"

#include <map>

SimulationInterface::SimulationInterface()
{
    mSimulation = 0;

    mWeightListSize = 200;
    mLastWeightIndex = 0;
    mTopLeftWeightList = new float[mWeightListSize];
    std::fill_n(mTopLeftWeightList, mWeightListSize, 0);
    mTopRightWeightList = new float[mWeightListSize];
    std::fill_n(mTopRightWeightList, mWeightListSize, 0);
    mBottomLeftWeightList = new float[mWeightListSize];
    std::fill_n(mBottomLeftWeightList, mWeightListSize, 0);
    mBottomRightWeightList = new float[mWeightListSize];
    std::fill_n(mBottomRightWeightList, mWeightListSize, 0);

    mNoActorThreshold = 5;
    mInitialDriverValue = 0.5;
    mFootRaiseFraction = 2.0;
    mMaxWiggleMod = 1.10;
    mWiggleModGain = 10;

}

SimulationInterface::~SimulationInterface()
{
    delete mTopLeftWeightList;
    delete mTopRightWeightList;
    delete mBottomLeftWeightList;
    delete mBottomRightWeightList;
}

void SimulationInterface::SetSimulation(Simulation *simulation)
{
    mSimulation = simulation;
    if (mSimulation == 0) return;

    // get all the required drivers
    std::map<std::string, Driver *> *driverList = mSimulation->GetDriverList();
    mDriverList.clear();
    mDriverList["LeftDeepDorsalGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftDeepDorsalGroupDriver"));
    mDriverList["LeftTricepsFemorisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftTricepsFemorisGroupDriver"));
    mDriverList["LeftCaudoFemoralisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftCaudoFemoralisGroupDriver"));
    mDriverList["LeftFemoroTibialisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftFemoroTibialisGroupDriver"));
    mDriverList["LeftFlexorCrurisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftFlexorCrurisGroupDriver"));
    mDriverList["LeftGastrocnemiusLateralis_FDDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftGastrocnemiusLateralis_FDDriver"));
    mDriverList["LeftTibialisAnterior_EDDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftTibialisAnterior_EDDriver"));
    mDriverList["LeftGastrocnemiusMedialisDriver"] = dynamic_cast<FixedDriver *>(driverList->at("LeftGastrocnemiusMedialisDriver"));
    mDriverList["RightDeepDorsalGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightDeepDorsalGroupDriver"));
    mDriverList["RightTricepsFemorisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightTricepsFemorisGroupDriver"));
    mDriverList["RightCaudoFemoralisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightCaudoFemoralisGroupDriver"));
    mDriverList["RightFemoroTibialisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightFemoroTibialisGroupDriver"));
    mDriverList["RightFlexorCrurisGroupDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightFlexorCrurisGroupDriver"));
    mDriverList["RightGastrocnemiusLateralis_FDDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightGastrocnemiusLateralis_FDDriver"));
    mDriverList["RightTibialisAnterior_EDDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightTibialisAnterior_EDDriver"));
    mDriverList["RightGastrocnemiusMedialisDriver"] = dynamic_cast<FixedDriver *>(driverList->at("RightGastrocnemiusMedialisDriver"));
}


void SimulationInterface::SetCornerWeights(float kgTopLeft, float kgTopRight, float kgBottomLeft, float kgBottomRight)
{
    // update the mean values
    mTopLeftWeightSum = mTopLeftWeightSum + kgTopLeft - mTopLeftWeightList[mLastWeightIndex];
    mTopLeftWeightAverage = mTopLeftWeightSum / mWeightListSize;
    mTopLeftWeightList[mLastWeightIndex] = kgTopLeft;

    mTopRightWeightSum = mTopRightWeightSum + kgTopRight - mTopRightWeightList[mLastWeightIndex];
    mTopRightWeightAverage = mTopRightWeightSum / mWeightListSize;
    mTopRightWeightList[mLastWeightIndex] = kgTopRight;

    mBottomLeftWeightSum = mBottomLeftWeightSum + kgBottomLeft - mBottomLeftWeightList[mLastWeightIndex];
    mBottomLeftWeightAverage = mBottomLeftWeightSum / mWeightListSize;
    mBottomLeftWeightList[mLastWeightIndex] = kgBottomLeft;

    mBottomRightWeightSum = mBottomRightWeightSum + kgBottomRight - mBottomRightWeightList[mLastWeightIndex];
    mBottomRightWeightAverage = mBottomRightWeightSum / mWeightListSize;
    mBottomRightWeightList[mLastWeightIndex] = kgBottomRight;

    mLastWeightIndex++;
    if (mLastWeightIndex >= mWeightListSize) mLastWeightIndex = 0;

    if (mSimulation == 0) return; // stop here is we don not have a simulation to activate

    // do we have an actor
    float sum = kgTopLeft + kgTopRight + kgBottomLeft + kgBottomRight;
    if (sum < mNoActorThreshold) // no actor present
    {
        // set all the drivers to zero
        for (std::map<std::string, FixedDriver *>::const_iterator iter = mDriverList.begin(); iter != mDriverList.end(); iter++) iter->second->SetValue(0);
        return;
    }

    SetDefaultActivations();

    float leftSum = kgTopLeft + kgBottomLeft;
    float rightSum = kgTopRight + kgBottomRight;
    float topSum = kgTopLeft + kgTopRight;
    float bottomSum = kgBottomLeft + kgBottomRight;
    float leftRightRatio = leftSum / rightSum;
    float topBottomRatio = topSum / bottomSum;

    //std::cerr << "leftRightRatio = " << leftRightRatio << "\n";
    //std::cerr << "topBottomRatio = " << topBottomRatio << "\n";

    // decide whether to lift up a foot
    if (leftRightRatio <  1 / mFootRaiseFraction) LegRaise(right);
    if (leftRightRatio > mFootRaiseFraction) LegRaise(left);

    // tweak the model so that it moves a little
    mDriverList["LeftDeepDorsalGroupDriver"]->ModValue(GenerateMod(kgTopLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip flexor
    mDriverList["LeftTricepsFemorisGroupDriver"]->ModValue(GenerateMod(kgTopLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip flexor, knee extensor
    mDriverList["LeftCaudoFemoralisGroupDriver"]->ModValue(GenerateMod(kgTopRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip extensor
    mDriverList["LeftFlexorCrurisGroupDriver"]->ModValue(GenerateMod(kgTopRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip extensor, knee flexor
    mDriverList["LeftFemoroTibialisGroupDriver"]->ModValue(GenerateMod(kgBottomLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // knee extensor
    mDriverList["LeftTibialisAnterior_EDDriver"]->ModValue(GenerateMod(kgBottomLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // ankle and foot flexor
    mDriverList["LeftGastrocnemiusLateralis_FDDriver"]->ModValue(GenerateMod(kgBottomRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // knee flexor, ankle extensor
    mDriverList["LeftGastrocnemiusMedialisDriver"]->ModValue(GenerateMod(kgBottomRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // ankle extensor
    mDriverList["RightDeepDorsalGroupDriver"]->ModValue(GenerateMod(kgTopLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip flexor
    mDriverList["RightTricepsFemorisGroupDriver"]->ModValue(GenerateMod(kgTopLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip flexor, knee extensor
    mDriverList["RightCaudoFemoralisGroupDriver"]->ModValue(GenerateMod(kgTopRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip extensor
    mDriverList["RightFlexorCrurisGroupDriver"]->ModValue(GenerateMod(kgTopRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // hip extensor, knee flexor
    mDriverList["RightFemoroTibialisGroupDriver"]->ModValue(GenerateMod(kgBottomLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // knee extensor
    mDriverList["RightTibialisAnterior_EDDriver"]->ModValue(GenerateMod(kgBottomLeft, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // ankle and foot flexor
    mDriverList["RightGastrocnemiusLateralis_FDDriver"]->ModValue(GenerateMod(kgBottomRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // knee flexor, ankle extensor
    mDriverList["RightGastrocnemiusMedialisDriver"]->ModValue(GenerateMod(kgBottomRight, mTopLeftWeightAverage, 1 / mMaxWiggleMod, mMaxWiggleMod)); // ankle extensor
}

float SimulationInterface::GenerateMod(float currentValue, float meanValue, float minMod, float maxMod)
{
    float mod = mWiggleModGain * currentValue / meanValue;
    if (mod < minMod) mod = minMod;
    if (mod > maxMod) mod = maxMod;
    return mod;
}

void SimulationInterface::SetDefaultActivations()
{
    // set all the drivers to mInitialDriverValue
    for (std::map<std::string, FixedDriver *>::const_iterator iter = mDriverList.begin(); iter != mDriverList.end(); iter++) iter->second->SetValue(mInitialDriverValue);
}

void SimulationInterface::LegRaise(Location location)
{
    if (mSimulation == 0) return;
    switch (location)
    {
    case left:
        //std::cerr << "Raise left leg\n";
        // raise left leg
        mDriverList["LeftDeepDorsalGroupDriver"]->SetValue(0.8); // hip flexor
        mDriverList["LeftTricepsFemorisGroupDriver"]->SetValue(0.6); // hip flexor, knee extensor
        mDriverList["LeftCaudoFemoralisGroupDriver"]->SetValue(0); // hip extensor
        mDriverList["LeftFlexorCrurisGroupDriver"]->SetValue(0.7); // hip extensor, knee flexor
        mDriverList["LeftFemoroTibialisGroupDriver"]->SetValue(0); // knee extensor
        mDriverList["LeftTibialisAnterior_EDDriver"]->SetValue(0.9); // ankle and foot flexor
        mDriverList["LeftGastrocnemiusLateralis_FDDriver"]->SetValue(0.6); // knee flexor, ankle extensor
        mDriverList["LeftGastrocnemiusMedialisDriver"]->SetValue(0); // ankle extensor
        // straighten right leg
        mDriverList["RightDeepDorsalGroupDriver"]->SetValue(0.3); // hip flexor
        mDriverList["RightTricepsFemorisGroupDriver"]->SetValue(0.8); // hip flexor, knee extensor
        mDriverList["RightCaudoFemoralisGroupDriver"]->SetValue(0.5); // hip extensor
        mDriverList["RightFlexorCrurisGroupDriver"]->SetValue(0); // hip extensor, knee flexor
        mDriverList["RightFemoroTibialisGroupDriver"]->SetValue(0.9); // knee extensor
        mDriverList["RightTibialisAnterior_EDDriver"]->SetValue(0); // ankle and foot flexor
        mDriverList["RightGastrocnemiusLateralis_FDDriver"]->SetValue(0.2); // knee flexor, ankle extensor
        mDriverList["RightGastrocnemiusMedialisDriver"]->SetValue(0.9); // ankle extensor
        break;

    case right:
        //std::cerr << "Raise right leg\n";
        // raise right leg
        mDriverList["RightDeepDorsalGroupDriver"]->SetValue(0.8); // hip flexor
        mDriverList["RightTricepsFemorisGroupDriver"]->SetValue(0.6); // hip flexor, knee extensor
        mDriverList["RightCaudoFemoralisGroupDriver"]->SetValue(0); // hip extensor
        mDriverList["RightFlexorCrurisGroupDriver"]->SetValue(0.7); // hip extensor, knee flexor
        mDriverList["RightFemoroTibialisGroupDriver"]->SetValue(0); // knee extensor
        mDriverList["RightTibialisAnterior_EDDriver"]->SetValue(1); // ankle and foot flexor
        mDriverList["RightGastrocnemiusLateralis_FDDriver"]->SetValue(0.6); // knee flexor, ankle extensor
        mDriverList["RightGastrocnemiusMedialisDriver"]->SetValue(0); // ankle extensor
        // straighten left leg
        mDriverList["LeftDeepDorsalGroupDriver"]->SetValue(0.3); // hip flexor
        mDriverList["LeftTricepsFemorisGroupDriver"]->SetValue(0.8); // hip flexor, knee extensor
        mDriverList["LeftCaudoFemoralisGroupDriver"]->SetValue(0.5); // hip extensor
        mDriverList["LeftFlexorCrurisGroupDriver"]->SetValue(0); // hip extensor, knee flexor
        mDriverList["LeftFemoroTibialisGroupDriver"]->SetValue(0.9); // knee extensor
        mDriverList["LeftTibialisAnterior_EDDriver"]->SetValue(0); // ankle and foot flexor
        mDriverList["LeftGastrocnemiusLateralis_FDDriver"]->SetValue(0.2); // knee flexor, ankle extensor
        mDriverList["LeftGastrocnemiusMedialisDriver"]->SetValue(1); // ankle extensor
        break;

    default:
        std::cerr << "Only two legs in this simulation\n";
    }
}
