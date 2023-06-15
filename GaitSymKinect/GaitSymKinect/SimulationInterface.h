/*
 *  SimulationInterface.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
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

    void LegRaise(Location location);
    void SetDefaultActivations();

    int SetDriverValue(const char *ID, double value);

protected:
    Simulation *mSimulation;

    float mInitialDriverValue;

    std::map<std::string, FixedDriver *> mDriverList;
};

#endif // SIMULATIONINTERFACE_H
