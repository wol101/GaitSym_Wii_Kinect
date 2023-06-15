/*
 *  Preferences.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>
#include <QByteArray>
#include <QSize>
#include <QPoint>
#include <QColor>

class Preferences
{
public:
    Preferences();

    void Read(bool resetToDefaults);
    void Write();

    QString Organisation; // organisation string - used for file names
    QString Application; // application name - used for file names
    QByteArray MainWindowGeometry; // window size and position information at exit
    QByteArray MainWindowState; // window state at exit
    double CameraDistance;
    double CameraFoV;
    double CameraCOIX;
    double CameraCOIY;
    double CameraCOIZ;
    double CameraVecX;
    double CameraVecY;
    double CameraVecZ;
    double CameraUpX;
    double CameraUpY;
    double CameraUpZ;
    double BodyDensity; // density used for calculations in cubic metres
    QColor BackgroundColour; // seclected background colour
    QColor EnvironmentColour;
    QColor BodyColour;
    QColor JointColour;
    QColor GeomColour;
    QColor GeomForceColour;
    QColor StrapColour;
    QColor StrapForceColour;
    QColor StrapCylinderColour;
    QColor ReporterColour;
    QColor DataTargetColour;
    double EnvironmentX;
    double EnvironmentY;
    double EnvironmentZ;
    double BodyX;
    double BodyY;
    double BodyZ;
    double JointX;
    double JointY;
    double JointZ;
    double GeomX;
    double GeomY;
    double GeomZ;
    double GeomForceScale;
    double GeomForceRadius;
    double StrapRadius;
    double StrapForceRadius;
    double StrapForceScale;
    double StrapCylinderLength;
    double ReporterX;
    double ReporterY;
    double ReporterZ;
    double DataTargetX;
    double DataTargetY;
    double DataTargetZ;
    double PresetCOIX;
    double PresetCOIY;
    double PresetCOIZ;
    int SensorInterval; // Kinect sensor read interval in milliseconds
    int FrameSkip; // simulation steps per display frame
    bool AutoLoadFlag; // if true, autoload last opened file
    bool TrackingFlag; // if true, the centre of interest tracks a specific body
    bool RestoreWindowsFlag; // if true, restore the window geometry and state
    bool FactorDefaultsFlag; // if set, load the factory defaults at start up
    QString LastFileName; // path of last loaded file
    int KinectJointDiameter; // joint size for kinect view
    int KinectBodyLineWidth; // body line width for kinect view
};

#endif // PREFERENCES_H
