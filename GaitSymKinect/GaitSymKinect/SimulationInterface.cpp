/*
 *  SimulationInterface.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "SimulationInterface.h"
#include "Simulation.h"
#include "FixedDriver.h"

#include <map>

SimulationInterface::SimulationInterface()
{
    mSimulation = 0;
    mInitialDriverValue = 0.5;
}

SimulationInterface::~SimulationInterface()
{
}

void SimulationInterface::SetSimulation(Simulation *simulation)
{
    mSimulation = simulation;
    if (mSimulation == 0) return;

    // get all the required drivers
    std::map<std::string, Driver *> *driverList = mSimulation->GetDriverList();
    FixedDriver *fixedDriver;
    mDriverList.clear();
    for (std::map<std::string, Driver *>::const_iterator iter = driverList->begin(); iter != driverList->end(); iter++)
    {
        fixedDriver = dynamic_cast<FixedDriver *>(iter->second);
        if (fixedDriver)
        {
            mDriverList[iter->first] = fixedDriver;
            // std::cerr << iter->first << "\n";
        }
    }
}


void SimulationInterface::SetDefaultActivations()
{
    // set all the drivers to mInitialDriverValue
    for (std::map<std::string, FixedDriver *>::const_iterator iter = mDriverList.begin(); iter != mDriverList.end(); iter++)
    {
        if (iter->first.find("Sky") == std::string::npos) // check name does not contain "Sky"
        {
            iter->second->SetValue(mInitialDriverValue);
        }
    }
}

void SimulationInterface::LegRaise(Location location)
{
    if (mSimulation == 0) return;
    switch (location)
    {
    case left:
        //std::cerr << "Raise left leg\n";
        // raise left leg
        mDriverList["LeftHipFlexDriver"]->SetValue(0.8); // hip flexor
        mDriverList["LeftHipFlexKneeExtDriver"]->SetValue(0.6); // hip flexor, knee extensor
        mDriverList["LeftHipExtDriver"]->SetValue(0); // hip extensor
        mDriverList["LeftHipExtKneeFlexDriver"]->SetValue(0.7); // hip extensor, knee flexor
        mDriverList["LeftKneeExtDriver"]->SetValue(0); // knee extensor
        mDriverList["LeftAnkleFlexDriver"]->SetValue(0.9); // ankle and foot flexor
        mDriverList["LeftAnkleExtKneeFlexDriver"]->SetValue(0.6); // knee flexor, ankle extensor
        mDriverList["LeftAnkleExtDriver"]->SetValue(0); // ankle extensor
        // straighten right leg
        mDriverList["RightHipFlexDriver"]->SetValue(0.0); // hip flexor
        mDriverList["RightHipFlexKneeExtDriver"]->SetValue(0.8); // hip flexor, knee extensor
        mDriverList["RightHipExtDriver"]->SetValue(0.9); // hip extensor
        mDriverList["RightHipExtKneeFlexDriver"]->SetValue(0); // hip extensor, knee flexor
        mDriverList["RightKneeExtDriver"]->SetValue(0.9); // knee extensor
        mDriverList["RightAnkleFlexDriver"]->SetValue(0); // ankle and foot flexor
        mDriverList["RightAnkleExtKneeFlexDriver"]->SetValue(0.2); // knee flexor, ankle extensor
        mDriverList["RightAnkleExtDriver"]->SetValue(0.9); // ankle extensor
        break;

    case right:
        //std::cerr << "Raise right leg\n";
        // raise right leg
        mDriverList["RightHipFlexDriver"]->SetValue(0.8); // hip flexor
        mDriverList["RightHipFlexKneeExtDriver"]->SetValue(0.6); // hip flexor, knee extensor
        mDriverList["RightHipExtDriver"]->SetValue(0); // hip extensor
        mDriverList["RightHipExtKneeFlexDriver"]->SetValue(0.7); // hip extensor, knee flexor
        mDriverList["RightKneeExtDriver"]->SetValue(0); // knee extensor
        mDriverList["RightAnkleFlexDriver"]->SetValue(0.9); // ankle and foot flexor
        mDriverList["RightAnkleExtKneeFlexDriver"]->SetValue(0.6); // knee flexor, ankle extensor
        mDriverList["RightAnkleExtDriver"]->SetValue(0); // ankle extensor
       // straighten left leg
        mDriverList["LeftHipFlexDriver"]->SetValue(0.0); // hip flexor
        mDriverList["LeftHipFlexKneeExtDriver"]->SetValue(0.8); // hip flexor, knee extensor
        mDriverList["LeftHipExtDriver"]->SetValue(0.9); // hip extensor
        mDriverList["LeftHipExtKneeFlexDriver"]->SetValue(0); // hip extensor, knee flexor
        mDriverList["LeftKneeExtDriver"]->SetValue(0.9); // knee extensor
        mDriverList["LeftAnkleFlexDriver"]->SetValue(0); // ankle and foot flexor
        mDriverList["LeftAnkleExtKneeFlexDriver"]->SetValue(0.2); // knee flexor, ankle extensor
        mDriverList["LeftAnkleExtDriver"]->SetValue(0.9); // ankle extensor
        break;

    default:
        std::cerr << "Only two legs in this simulation\n";
    }
}

// this function safely sets a driver value only if it exists
int SimulationInterface::SetDriverValue(const char *ID, double value)
{
    if (mSimulation == 0) return __LINE__;
    std::map<std::string, FixedDriver *>::const_iterator iter = mDriverList.find(std::string(ID));
    if (iter != mDriverList.end())
    {
        iter->second->SetValue(value);
        return 0;
    }
    return __LINE__;
}


