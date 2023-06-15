/*
 *  MainWindow.h
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTimer;
class QGridLayout;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class Preferences;
class GLWidget;
class SimulationInterface;
class KinectConnector;
class KinectInterface;
class AnatomicalCharacter;

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
    const Preferences *GetPreferences() { return mPreferences; }

public slots:
    void UpdateKinect();
    void SetPreferences();
    void AboutBox();
    void OpenConfig();
    void SaveConfig();
    void SaveAsConfig();
    void ProcessOneThing();
    void LeftFootButton();
    void RightFootButton();
    void RightViewButton();
    void TopViewButton();
    void FrontViewButton();
    void PresetCOIButton();
    void FullScreenButton();
    void HideToolbarButton();
    void MenuRequestTableWidgetJointAngles(const QPoint &p);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void OutputValue();
    void InitialiseJointAngleFrame();
    void UpdateJointAngleFrame();

    Ui::MainWindow *ui;

    QTimer *mProcessOneThingTimer;
    long long mStepCount;
    long long mSensorCount;

    QTimer *mUpdateKinectTimer;
    Preferences *mPreferences;

    GLWidget *mGLWidget;
    QGraphicsView *mGraphicsView;
    QGraphicsScene *mGraphicsScene;
    QGraphicsPixmapItem *mPixmapItem;

    SimulationInterface *mSimulationInterface;
    KinectInterface *mKinect;
    int mActiveCharacter;
    KinectConnector *mkinectConnector;
    AnatomicalCharacter *mAnatomicalCharacter;

};

#endif // MAINWINDOW_H
