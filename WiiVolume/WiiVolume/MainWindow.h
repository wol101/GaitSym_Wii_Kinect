/*
 *  MainWindow.h
 *  WiiVolume
 *
 *  Created by Bill Sellers on 12/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef USE_WIIC
#include "wiic/wiic.h"
#else
#ifdef USE_WIIUSE
#include <wiiuse.h>
#endif
#endif

#include <QMainWindow>

class QTimer;
class QGridLayout;
class Preferences;
class GLWidget;
class SimulationInterface;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);
    void ReadConfig(const QString &fileName);
    void SetDefaultLights(bool yUp);
    void WriteSettings();

public slots:
    void FindWiimotes();
    void PollWiiMotes();
    void SetPreferences();
    void OpenConfig();
    void ProcessOneThing();
    void SetOutputWeight();
    void SetOutputVolume();
    void LeftFootButton();
    void RightFootButton();
    void RightViewButton();
    void TopViewButton();
    void FrontViewButton();
    void PresetCOIButton();
    void FullScreenButton();
    void HideToolbarButton();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void print_event(struct wiimote_t* wm);
    void OutputValue();

    Ui::MainWindow *ui;

    QTimer *mProcessOneThingTimer;
    long long mStepCount;

    QTimer *mWiiPollTimer;
    wiimote **mWiimotes;
    int mFound;
    int mConnected;
#ifdef USE_WIIC
    struct balance_board_t* mBB;
#else
#ifdef USE_WIIUSE
    struct wii_board_t* mBB;
#endif
#endif
    Preferences *mPreferences;

    int mLastWeightIndex;
    float *mWeightList;
    float mWeightSum;
    float mWeightAverage;

    GLWidget *mGLWidget;

    static const int MAX_WIIMOTES = 4;

    SimulationInterface *mSimulationInterface;
};

#endif // MAINWINDOW_H
