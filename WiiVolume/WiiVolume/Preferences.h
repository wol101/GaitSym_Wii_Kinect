/*
 *  Preferences.h
 *  WiiVolume
 *
 *  Created by Bill Sellers on 12/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
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

    void Read();
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
    double PresetCOIX;
    double PresetCOIY;
    double PresetCOIZ;
    int DisplayMode; // 0: weight, 1: volume
    int SampleInterval; // sample interbal in milliseconds
    int MovingAverageWindow; // the window size for the moving average calculations
    int FrameSkip; // simulation steps per display frame
    bool AutoLoadFlag; // if true, autoload last opened file
    QString LastFileName; // path of last loaded file
};

#endif // PREFERENCES_H
