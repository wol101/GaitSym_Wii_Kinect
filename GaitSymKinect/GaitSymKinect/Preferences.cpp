/*
 *  Preferences.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "Preferences.h"

#include <QSettings>

Preferences::Preferences()
{
    Organisation = "AnimalSimulationLaboratory";
    Application = "GaitSymKinect";
}

void Preferences::Read(bool resetToDefaults)
{
    QSettings settings(Organisation, Application);

    FactorDefaultsFlag = settings.value("FactorDefaultsFlag", false).toBool();
    if (resetToDefaults || FactorDefaultsFlag) settings.clear();

    MainWindowGeometry = settings.value("MainWindowGeometry").toByteArray();
    MainWindowState = settings.value("MainWindowState").toByteArray();
    CameraDistance = settings.value("CameraDistance", 50).toDouble();
    CameraFoV = settings.value("CameraFoV", 5).toDouble();
    CameraCOIX = settings.value("CameraCOIX", 0).toDouble();
    CameraCOIY = settings.value("CameraCOIY", 0).toDouble();
    CameraCOIZ = settings.value("CameraCOIZ", 0).toDouble();
    CameraVecX = settings.value("CameraVecX", 0).toDouble();
    CameraVecY = settings.value("CameraVecY", 1).toDouble();
    CameraVecZ = settings.value("CameraVecZ", 0).toDouble();
    CameraUpX = settings.value("CameraUpX", 0).toDouble();
    CameraUpY = settings.value("CameraUpY", 0).toDouble();
    CameraUpZ = settings.value("CameraUpZ", 1).toDouble();
    PresetCOIX = settings.value("PresetCOIX", 0).toDouble();
    PresetCOIY = settings.value("PresetCOIY", 0).toDouble();
    PresetCOIZ = settings.value("PresetCOIZ", 0).toDouble();

    BodyDensity = settings.value("BodyDensity", 1000).toDouble();
    SensorInterval = settings.value("SensorInterval", 100).toInt();
    FrameSkip = settings.value("FrameSkip", 1000).toInt();
    EnvironmentX = settings.value("EnvironmentX", 1).toDouble();
    EnvironmentY = settings.value("EnvironmentY", 1).toDouble();
    EnvironmentZ = settings.value("EnvironmentZ", 1).toDouble();
    BodyX = settings.value("BodyX", 0.1).toDouble();
    BodyY = settings.value("BodyY", 0.1).toDouble();
    BodyZ = settings.value("BodyZ", 0.1).toDouble();
    JointX = settings.value("JointX", 0.05).toDouble();
    JointY = settings.value("JointY", 0.05).toDouble();
    JointZ = settings.value("JointZ", 0.05).toDouble();
    GeomX = settings.value("GeomX", 0.05).toDouble();
    GeomY = settings.value("GeomY", 0.05).toDouble();
    GeomZ = settings.value("GeomZ", 0.05).toDouble();
    GeomForceScale = settings.value("GeomForceScale", 0.00001).toDouble();
    GeomForceRadius = settings.value("GeomForceRadius", 0.01).toDouble();
    StrapRadius = settings.value("StrapRadius", 0.005).toDouble();
    StrapForceRadius = settings.value("StrapForceRadius", 0.01).toDouble();
    StrapForceScale = settings.value("StrapForceScale", 0.00001).toDouble();
    StrapCylinderLength = settings.value("StrapCylinderLength", 0.04).toDouble();
    ReporterX = settings.value("ReporterX", 0.005).toDouble();
    ReporterY = settings.value("ReporterY", 0.005).toDouble();
    ReporterZ = settings.value("ReporterZ", 0.005).toDouble();
    DataTargetX = settings.value("DataTargetX", 0.005).toDouble();
    DataTargetY = settings.value("DataTargetY", 0.005).toDouble();
    DataTargetZ = settings.value("DataTargetZ", 0.005).toDouble();

    AutoLoadFlag = settings.value("AutoLoadFlag", false).toBool();
    TrackingFlag = settings.value("TrackingFlag", false).toBool();
    RestoreWindowsFlag = settings.value("RestoreWindowsFlag", false).toBool();
    LastFileName = settings.value("LastFileName", "").toByteArray();

    // handle default Colors
    if (settings.contains("BackgroundColour") == false) settings.setValue("BackgroundColour", QColor(255, 255, 255, 255));
    if (settings.contains("EnvironmentColour") == false) settings.setValue("EnvironmentColour", QColor(0,255,255));
    if (settings.contains("BodyColour") == false) settings.setValue("BodyColour", QColor(0,255,0));
    if (settings.contains("JointColour") == false) settings.setValue("JointColour", QColor(0,0,255));
    if (settings.contains("GeomColour") == false) settings.setValue("GeomColour", QColor(255,255,0));
    if (settings.contains("GeomForceColour") == false) settings.setValue("GeomForceColour", QColor(255,255,0));
    if (settings.contains("StrapColour") == false) settings.setValue("StrapColour", QColor(255,0,0));
    if (settings.contains("StrapForceColour") == false) settings.setValue("StrapForceColour", QColor(255,0,0));
    if (settings.contains("StrapCylinderColour") == false) settings.setValue("StrapCylinderColour", QColor(255,0,255));
    if (settings.contains("ReporterColour") == false) settings.setValue("ReporterColour", QColor(255,128,0));
    if (settings.contains("DataTargetColour") == false) settings.setValue("DataTargetColour", QColor(255,128,128));

    BackgroundColour = settings.value("BackgroundColour").value<QColor>();
    EnvironmentColour = settings.value("EnvironmentColour").value<QColor>();
    BodyColour = settings.value("BodyColour").value<QColor>();
    JointColour = settings.value("JointColour").value<QColor>();
    GeomColour = settings.value("GeomColour").value<QColor>();
    GeomForceColour = settings.value("GeomForceColour").value<QColor>();
    StrapColour = settings.value("StrapColour").value<QColor>();
    StrapForceColour = settings.value("StrapForceColour").value<QColor>();
    StrapCylinderColour = settings.value("StrapCylinderColour").value<QColor>();
    ReporterColour = settings.value("ReporterColour").value<QColor>();
    DataTargetColour = settings.value("DataTargetColour").value<QColor>();

    KinectJointDiameter = settings.value("KinectJointDiameter", 16).toInt();
    KinectBodyLineWidth = settings.value("KinectBodyLineWidth", 4).toInt();
}

void Preferences::Write()
{
    QSettings settings(Organisation, Application);

    settings.setValue("MainWindowGeometry", MainWindowGeometry);
    settings.setValue("MainWindowState", MainWindowState);
    settings.setValue("CameraDistance", CameraDistance);
    settings.setValue("CameraFoV", CameraFoV);
    settings.setValue("CameraCOIX", CameraCOIX);
    settings.setValue("CameraCOIY", CameraCOIY);
    settings.setValue("CameraCOIZ", CameraCOIZ);
    settings.setValue("CameraVecX", CameraVecX);
    settings.setValue("CameraVecY", CameraVecY);
    settings.setValue("CameraVecZ", CameraVecZ);
    settings.setValue("CameraUpX", CameraUpX);
    settings.setValue("CameraUpY", CameraUpY);
    settings.setValue("CameraUpZ", CameraUpZ);
    settings.setValue("PresetCOIX", PresetCOIX);
    settings.setValue("PresetCOIY", PresetCOIY);
    settings.setValue("PresetCOIZ", PresetCOIZ);

    settings.setValue("BodyDensity", BodyDensity);
    settings.setValue("SensorInterval", SensorInterval);
    settings.setValue("FrameSkip", FrameSkip);

    settings.setValue("EnvironmentX", EnvironmentX);
    settings.setValue("EnvironmentY", EnvironmentY);
    settings.setValue("EnvironmentZ", EnvironmentZ);
    settings.setValue("BodyX", BodyX);
    settings.setValue("BodyY", BodyY);
    settings.setValue("BodyZ", BodyZ);
    settings.setValue("JointX", JointX);
    settings.setValue("JointY", JointY);
    settings.setValue("JointZ", JointZ);
    settings.setValue("GeomX", GeomX);
    settings.setValue("GeomY", GeomY);
    settings.setValue("GeomZ", GeomZ);
    settings.setValue("GeomForceScale", GeomForceScale);
    settings.setValue("GeomForceRadius", GeomForceRadius);
    settings.setValue("StrapRadius", StrapRadius);
    settings.setValue("StrapForceRadius", StrapForceRadius);
    settings.setValue("StrapForceScale", StrapForceScale);
    settings.setValue("StrapCylinderLength", StrapCylinderLength);
    settings.setValue("ReporterX", ReporterX);
    settings.setValue("ReporterY", ReporterY);
    settings.setValue("ReporterZ", ReporterZ);

    settings.setValue("AutoLoadFlag", AutoLoadFlag);
    settings.setValue("TrackingFlag", TrackingFlag);
    settings.setValue("RestoreWindowsFlag", RestoreWindowsFlag);
    settings.setValue("FactorDefaultsFlag", FactorDefaultsFlag);
    settings.setValue("LastFileName", LastFileName);

    settings.setValue("BackgroundColour", BackgroundColour);
    settings.setValue("EnvironmentColour", EnvironmentColour);
    settings.setValue("BodyColour", BodyColour);
    settings.setValue("JointColour", JointColour);
    settings.setValue("GeomColour", GeomColour);
    settings.setValue("GeomForceColour", GeomForceColour);
    settings.setValue("StrapColour", StrapColour);
    settings.setValue("StrapForceColour", StrapForceColour);
    settings.setValue("StrapCylinderColour", StrapCylinderColour);
    settings.setValue("ReporterColour", ReporterColour);

    settings.setValue("KinectJointDiameter", KinectJointDiameter);
    settings.setValue("KinectBodyLineWidth", KinectBodyLineWidth);

    settings.sync();
}
