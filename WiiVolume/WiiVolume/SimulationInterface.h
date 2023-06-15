/*
 *  SimulationInterface.h
 *  WiiVolume
 *
 *  Created by Bill Sellers on 21/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#ifndef SIMULATIONINTERFACE_H
#define SIMULATIONINTERFACE_H

#include <string>
#include <map>

class Simulation;
class FixedDriver;

class SimulationInterface
{
public:
    SimulationInterface();
    ~SimulationInterface();

    enum Location {left, right, front, back};

    void SetSimulation(Simulation *simulation);
    void SetCornerWeights(float kgTopLeft, float kgTopRight, float kgBottomLeft, float kgBottomRight);

    float GenerateMod(float currentValue, float meanValue, float minMod, float maxMod);
    void LegRaise(Location location);
    void SetDefaultActivations();

protected:
    Simulation *mSimulation;

    int mWeightListSize;
    int mLastWeightIndex;
    float *mTopLeftWeightList;
    float mTopLeftWeightSum;
    float mTopLeftWeightAverage;
    float *mTopRightWeightList;
    float mTopRightWeightSum;
    float mTopRightWeightAverage;
    float *mBottomLeftWeightList;
    float mBottomLeftWeightSum;
    float mBottomLeftWeightAverage;
    float *mBottomRightWeightList;
    float mBottomRightWeightSum;
    float mBottomRightWeightAverage;

    float mNoActorThreshold;
    float mInitialDriverValue;
    float mFootRaiseFraction;
    float mMaxWiggleMod;
    float mWiggleModGain;

    std::map<std::string, FixedDriver *> mDriverList;
};

#endif // SIMULATIONINTERFACE_H
