/*
 *  MainWindow.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Preferences.h"
#include "PreferencesDialog.h"
#include "GLWidget.h"
#include "Simulation.h"
#include "ObjectiveMain.h"
#include "Body.h"
#include "SimulationInterface.h"
#include "KinectInterface.h"
#include "KinectConnector.h"
#include "KinectConnectorDialog.h"
#include "AboutDialog.h"
#include "PGDMath.h"
#include "AnatomicalCharacter.h"

#include <QTimer>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QPalette>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QList>

#include <algorithm>
#include <iostream>
#include <float.h>

// Simulation globals
extern Simulation *gSimulation;
extern char *gConfigFilenamePtr;
extern char *gGraphicsRoot;
// QSettings *settings = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mStepCount = 0;
    mSensorCount = 0;
    mPixmapItem = 0;
    mActiveCharacter = -1;
    mkinectConnector = new KinectConnector;
    mAnatomicalCharacter = new AnatomicalCharacter;

    // read in the preferences file
    mPreferences = new Preferences();
    mPreferences->Read(false);
    if (mPreferences->RestoreWindowsFlag)
    {
        if (mPreferences->MainWindowGeometry.size() > 0) restoreGeometry(mPreferences->MainWindowGeometry);
        if (mPreferences->MainWindowState.size() > 0) restoreState(mPreferences->MainWindowState);
    }

    mSimulationInterface = new SimulationInterface();

    // put GLWidget into the Frame via a QGridLayout
    QGridLayout *gridLayout = new QGridLayout(ui->frameOpenGL);
    gridLayout->setMargin(1);
    gridLayout->setSpacing(1);

    mGLWidget = new GLWidget(this);
    gridLayout->addWidget(mGLWidget, 0, 0);
    SetDefaultLights(false);

    // put graphics view into the Frame via a QGridLayout
    QGridLayout *gridLayout2 = new QGridLayout(ui->frameGraphicsView);
    gridLayout2->setMargin(1);
    gridLayout2->setSpacing(1);

    mGraphicsView = new QGraphicsView(this);
    mGraphicsScene = new QGraphicsScene(this);
    mGraphicsView->setScene(mGraphicsScene);
    gridLayout2->addWidget(mGraphicsView, 0, 0);

    // fill out the Joint Angle screen
    InitialiseJointAngleFrame();

    // set up the Kinect interface
    mKinect = new KinectInterface();
    mUpdateKinectTimer = new QTimer(this);
    connect(mUpdateKinectTimer, SIGNAL(timeout()), this, SLOT(UpdateKinect()));
    if (mPreferences->SensorInterval < 0) mPreferences->SensorInterval = 0;
    mUpdateKinectTimer->start(mPreferences->SensorInterval); // currently sample 10 times a second (100ms interval)

    // set up the process one thing timer
    mProcessOneThingTimer = new QTimer(this);
    connect(mProcessOneThingTimer, SIGNAL(timeout()), this, SLOT(ProcessOneThing()));

    // hook up the various button actions
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(SetPreferences()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(AboutBox()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenConfig()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveConfig()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAsConfig()));
    connect(ui->actionOpenToolbar, SIGNAL(triggered()), this, SLOT(OpenConfig()));
    connect(ui->actionLeft_Foot, SIGNAL(triggered()), this, SLOT(LeftFootButton()));
    connect(ui->actionRight_Foot, SIGNAL(triggered()), this, SLOT(RightFootButton()));
    connect(ui->actionRight_View, SIGNAL(triggered()), this, SLOT(RightViewButton()));
    connect(ui->actionFront_View, SIGNAL(triggered()), this, SLOT(FrontViewButton()));
    connect(ui->actionTop_View, SIGNAL(triggered()), this, SLOT(TopViewButton()));
    connect(ui->actionPreset_Centre_of_Interest, SIGNAL(triggered()), this, SLOT(PresetCOIButton()));
    connect(ui->actionFull_Screen, SIGNAL(triggered()), this, SLOT(FullScreenButton()));
    connect(ui->actionHide_Toolbar, SIGNAL(triggered()), this, SLOT(HideToolbarButton()));

    // custom menu on QTableWidget
    ui->tableWidgetJointAngles->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetJointAngles, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(MenuRequestTableWidgetJointAngles(const QPoint &)));

    mGLWidget->SetCameraDistance(mPreferences->CameraDistance);
    mGLWidget->SetCameraFoV(mPreferences->CameraFoV);
    mGLWidget->SetCameraCOIX(mPreferences->CameraCOIX);
    mGLWidget->SetCameraCOIY(mPreferences->CameraCOIY);
    mGLWidget->SetCameraCOIZ(mPreferences->CameraCOIZ);
    mGLWidget->SetCameraVec(mPreferences->CameraVecX, mPreferences->CameraVecY, mPreferences->CameraVecZ);
    mGLWidget->SetCameraUp(mPreferences->CameraUpX, mPreferences->CameraUpY, mPreferences->CameraUpZ);
    mGLWidget->SetBackground(mPreferences->BackgroundColour.redF(), mPreferences->BackgroundColour.greenF(), mPreferences->BackgroundColour.blueF(), mPreferences->BackgroundColour.alphaF());

    // and if AutoLoad is set, load up a new simulation
    if (mPreferences->AutoLoadFlag)
    {
        if (QFile(mPreferences->LastFileName).exists()) ReadConfig(mPreferences->LastFileName);
    }

}

MainWindow::~MainWindow()
{
    delete mkinectConnector;

    delete mProcessOneThingTimer;
    delete mUpdateKinectTimer;
    delete mKinect;
    delete mPreferences;
    delete mSimulationInterface;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    WriteSettings();

    QMainWindow::closeEvent(event);
}

void MainWindow::OpenConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open GaitSym File", mPreferences->LastFileName, "Config Files (*.xml)", 0, 0);
    if (fileName.isNull() == false)
    {
        ReadConfig(fileName);
    }
}

void MainWindow::ReadConfig(const QString &fileName)
{
    // create the simulation
    if (gSimulation) delete gSimulation;

    char *readFileName = new char[fileName.toUtf8().length() + 1];
    strcpy(readFileName, fileName.toUtf8());
    gConfigFilenamePtr = readFileName;

    char *graphicsRoot = new char[16];
    strcpy(graphicsRoot, "");
    gGraphicsRoot = graphicsRoot;

    QDir::setCurrent(QFileInfo(fileName).absolutePath());

    int err = ReadModel(this);
    delete [] graphicsRoot;
    delete [] readFileName;
    if (err)
    {
        statusBar()->showMessage(QString("Error loading ") + fileName);
        return;
    }
    // and start the simulation
    if (gSimulation)
    {
        mSimulationInterface->SetSimulation(gSimulation);
        mProcessOneThingTimer->start();
    }
    mPreferences->LastFileName = fileName;
    WriteSettings();
    ui->actionSave->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    mkinectConnector->Read(fileName); // this just reads the kinect connector data
}

void MainWindow::SaveConfig()
{
    // actually save config only saves the Kinect information
    if (mkinectConnector->Write(mPreferences->LastFileName))
    {
        statusBar()->showMessage(QString("Error saving: ") + mPreferences->LastFileName);
    }
    else
    {
        statusBar()->showMessage(QString("Saved: ") + mPreferences->LastFileName);
    }
}

void MainWindow::SaveAsConfig()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save GaitSym File"), mPreferences->LastFileName, "Config Files (*.xml)");
    if (fileName.isNull() == false)
    {
        mPreferences->LastFileName = fileName;
        WriteSettings();
        SaveConfig();
    }
}


void MainWindow::ProcessOneThing()
{
    if (gSimulation)
    {
        if (gSimulation->ShouldQuit() || gSimulation->TestForCatastrophy())
        {
            statusBar()->showMessage(tr("Unable to start simulation"));
            //ui->toolButtonPlay->setChecked(false);
            //run();
            return;
        }

        gSimulation->UpdateSimulation();
        mStepCount++;
        // std::cerr << mStepCount << "\n";
        bool trackingFlag = mPreferences->TrackingFlag;

        if ((mStepCount % mPreferences->FrameSkip) == 0)
        {
            Body *body = gSimulation->GetBody(gSimulation->GetInterface()->TrackBodyID.c_str());
            if (trackingFlag)
            {
                if (body)
                {
                    const double *position = dBodyGetPosition(body->GetBodyID());
                    mGLWidget->SetCameraCOIX(position[0]);
                }
            }
            mGLWidget->updateGL();
            const double *velocity = dBodyGetLinearVel(body->GetBodyID());
            ui->lcdNumberOutput->display(QString("%1").arg(velocity[0], 0, 'f', 1, ' '));
        }

        if (gSimulation->ShouldQuit())
        {
            statusBar()->showMessage(tr("Simulation ended normally"));
            mGLWidget->updateGL();
            mProcessOneThingTimer->stop();
            //QString time = QString("%1").arg(gSimulation->GetTime(), 0, 'f', 5);
            //ui->lcdNumberTime->display(time);
            return;
        }
        if (gSimulation->TestForCatastrophy())
        {
            statusBar()->showMessage(tr("Simulation aborted"));
            mGLWidget->updateGL();
            mProcessOneThingTimer->stop();
            //QString time = QString("%1").arg(gSimulation->GetTime(), 0, 'f', 5);
            return;
        }
    }
}

void MainWindow::SetPreferences()
{
    PreferencesDialog dialogPreferences(this);
    dialogPreferences.setValues(*mPreferences);
    // hook up the preferences dialog to the GLWidget
    connect(&dialogPreferences, SIGNAL(EmitBackgroundColour(float, float, float, float)), mGLWidget, SLOT(SetBackground(float, float, float, float)));

    int status = dialogPreferences.exec();
    if (status == QDialog::Accepted)
    {
        dialogPreferences.getValues(mPreferences);
        WriteSettings();
    }
}

void MainWindow::AboutBox()
{
    AboutDialog aboutDialog(this);

    int status = aboutDialog.exec();

    if (status == QDialog::Accepted)
    {
    }
}

void MainWindow::WriteSettings()
{
    mPreferences->CameraDistance = mGLWidget->GetCameraDistance();
    mPreferences->CameraFoV = mGLWidget->GetCameraFoV();
    mPreferences->CameraCOIX = mGLWidget->GetCameraCOIX();
    mPreferences->CameraCOIY = mGLWidget->GetCameraCOIY();
    mPreferences->CameraCOIZ = mGLWidget->GetCameraCOIZ();
    GLfloat x, y, z;
    mGLWidget->GetCameraVec(&x, &y, &z);
    mPreferences->CameraVecX = x;
    mPreferences->CameraVecY = y;
    mPreferences->CameraVecZ = z;
    mGLWidget->GetCameraUp(&x, &y, &z);
    mPreferences->CameraUpX = x;
    mPreferences->CameraUpY = y;
    mPreferences->CameraUpZ = z;

    mPreferences->MainWindowGeometry = saveGeometry();
    mPreferences->MainWindowState = saveState();
    mPreferences->Write();
}

void MainWindow::SetDefaultLights(bool yUp)
{
    // lights
    Light l0, l1, l2, l3;

    // Green front right
    l0.SetAmbient (0.2, 0.2, 0.2, 1);
    l0.SetDiffuse (0.3, 0.6, 0.3, 1);
    l0.SetSpecular(0.4, 0.6, 0.4, 1);
    if (yUp) l0.SetPosition(0.5, 1, 1, 0);
    else l0.SetPosition(0.5, -1, 1, 0);

    // Blue front left
    l1.SetAmbient (0.2, 0.2, 0.2, 1);
    l1.SetDiffuse (0.3, 0.3, 0.6, 1);
    l1.SetSpecular(0.4, 0.4, 0.6, 1);
    if (yUp) l1.SetPosition(0.5, 1, -1, 0);
    else l1.SetPosition(0.5, 1, 1, 0);

    // Red back centre
    l2.SetAmbient (0.2, 0.2, 0.2, 1);
    l2.SetDiffuse (0.6, 0.3, 0.3, 1);
    l2.SetSpecular(0.6, 0.4, 0.4, 1);
    if (yUp) l2.SetPosition(-1, 0, 0, 0);
    else l2.SetPosition(-1, 0, 0, 0);

    // White underneath
    l3.SetAmbient (0.1, 0.1, 0.1, 1);
    l3.SetDiffuse (0.2, 0.2, 0.2, 1);
    l3.SetSpecular(0.3, 0.3, 0.3, 1);
    if (yUp) l3.SetPosition(0, -1, 0, 0);
    else l3.SetPosition(0, 0, -1, 0);

    mGLWidget->ClearLights();
    mGLWidget->AddLight(&l0);
    mGLWidget->AddLight(&l1);
    mGLWidget->AddLight(&l2);
    mGLWidget->AddLight(&l3);
}

void MainWindow::LeftFootButton()
{
    if (gSimulation == 0) return;

    if (ui->actionLeft_Foot->isChecked() == true)
    {
        mSimulationInterface->LegRaise(SimulationInterface::right);
        ui->actionRight_Foot->setChecked(false);
    }
    else
    {
        mSimulationInterface->SetDefaultActivations();
        ui->actionLeft_Foot->setChecked(false);
    }
}

void MainWindow::RightFootButton()
{
    if (gSimulation == 0) return;

    if (ui->actionRight_Foot->isChecked() == true)
    {
        mSimulationInterface->LegRaise(SimulationInterface::left);
        ui->actionLeft_Foot->setChecked(false);
    }
    else
    {
        mSimulationInterface->SetDefaultActivations();
        ui->actionRight_Foot->setChecked(false);
    }
}

void MainWindow::RightViewButton()
{
    mGLWidget->SetCameraRight();
    mGLWidget->updateGL();
}


void MainWindow::TopViewButton()
{
    mGLWidget->SetCameraTop();
    mGLWidget->updateGL();
}


void MainWindow::FrontViewButton()
{
    mGLWidget->SetCameraFront();
    mGLWidget->updateGL();
}

void MainWindow::PresetCOIButton()
{
    mGLWidget->SetCameraCOIX(mPreferences->PresetCOIX);
    mGLWidget->SetCameraCOIY(mPreferences->PresetCOIY);
    mGLWidget->SetCameraCOIZ(mPreferences->PresetCOIZ);
    mGLWidget->updateGL();
}

void MainWindow::FullScreenButton()
{
    if (windowState() == Qt::WindowFullScreen)
    {
        setWindowState(Qt::WindowNoState);
    }
    else
    {
        setWindowState(Qt::WindowFullScreen);
    }
}

// handle the resize caused by full screen
void MainWindow::HideToolbarButton()
{
   if (ui->mainToolBar->isHidden())
   {
       ui->mainToolBar->show();
#if defined(Q_OS_LINUX)
       ui->menuBar->show();
#endif
   }
   else
   {
       ui->mainToolBar->hide();
#if defined(Q_OS_LINUX)
       ui->menuBar->hide();
#endif
   }
}


// handle key presses
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {

    // F is the full screen key
case Qt::Key_F:
    FullScreenButton();
    break;

    // H is the hide tool bar key
case Qt::Key_H:
    HideToolbarButton();
    break;

    default:
        QMainWindow::keyPressEvent( e );
    }
}

void MainWindow::UpdateKinect()
{
#if defined(_WIN32) || defined(WIN32)
//    static int colourList[12] = {Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
//                               Qt::darkRed, Qt::darkGreen, Qt:darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
    static int colourList[12] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

    mSensorCount++;
    // std::cerr << mSensorCount << "\n";
    mKinect->UpdateKinect();

    // and draw the depth map
    mGraphicsScene->clear(); // this removes and deletes all the items in the scene
    QImage* img = new QImage(KinectInterface::cDepthWidth, KinectInterface::cDepthHeight, QImage::Format_RGBA8888);
    const RGBQUAD *depthPtr = mKinect->GetDepthRGBX();
    for (int y = 0; y < img->height(); y++)
    {
        memcpy(img->scanLine(y), depthPtr, img->bytesPerLine());
        depthPtr += KinectInterface::cDepthWidth;
    }
    mPixmapItem = mGraphicsScene->addPixmap(QPixmap::fromImage(*img));
    delete img;
    mGraphicsView->fitInView(mPixmapItem, Qt::KeepAspectRatio);
    mGraphicsScene->setSceneRect(mPixmapItem->boundingRect());

    // and draw all the currently tracked characters
    int iCharacter, iBody;
    const KinectBody *kinectBody;
    const KinectJoint *joint1, *joint2;
    qreal jointRadius = qreal(mPreferences->KinectJointDiameter) / 2.0;
    for (iCharacter = 0; iCharacter <  mKinect->GetMaxCharacters(); iCharacter++)
    {
        if (mKinect->IsTracking(iCharacter) == true)
        {
            QPen pen(static_cast<Qt::GlobalColor>(colourList[iCharacter]));
            QBrush brush(static_cast<Qt::GlobalColor>(colourList[iCharacter]));
            for (iBody = 0; iBody < (int)KinectBodyIDCount; iBody++)
            {
                kinectBody = mKinect->GetBodyData(iCharacter, static_cast<KinectBodyID>(iBody));
                joint1 = mKinect->GetJointData(iCharacter, kinectBody->jointID1);
                joint2 = mKinect->GetJointData(iCharacter, kinectBody->jointID2);
                // only draw joints if they are within the depth image
                if (joint1->depthSpaceLocation.x >= 0 && joint1->depthSpaceLocation.x < KinectInterface::cDepthWidth &&
                    joint1->depthSpaceLocation.y >= 0 && joint1->depthSpaceLocation.y < KinectInterface::cDepthHeight &&
                    joint2->depthSpaceLocation.x >= 0 && joint2->depthSpaceLocation.x < KinectInterface::cDepthWidth &&
                    joint2->depthSpaceLocation.y >= 0 && joint2->depthSpaceLocation.y < KinectInterface::cDepthHeight)
                {
                    pen.setWidth(mPreferences->KinectBodyLineWidth);
                    mGraphicsScene->addLine(joint1->depthSpaceLocation.x, joint1->depthSpaceLocation.y,
                                            joint2->depthSpaceLocation.x, joint2->depthSpaceLocation.y, pen);
                    pen.setWidth(1);
                    mGraphicsScene->addEllipse(joint1->depthSpaceLocation.x - jointRadius,
                                               joint1->depthSpaceLocation.y - jointRadius,
                                               mPreferences->KinectJointDiameter, mPreferences->KinectJointDiameter, pen, brush);
                    mGraphicsScene->addEllipse(joint2->depthSpaceLocation.x - jointRadius,
                                               joint2->depthSpaceLocation.y - jointRadius,
                                               mPreferences->KinectJointDiameter, mPreferences->KinectJointDiameter, pen, brush);
                    // qDebug("Joint %f %f\n", joint1->depthSpaceLocation.x, joint1->depthSpaceLocation.y);
                }
            }
        }
    }


    // first find character of interest
    int closestCharacter = -1;
    double closestZ = DBL_MAX;
    const KinectJoint *bodyPosition;
    for (iCharacter = 0; iCharacter <  mKinect->GetMaxCharacters(); iCharacter++)
    {
        if (mKinect->IsTracking(iCharacter) == true)
        {
            bodyPosition = mKinect->GetJointData(iCharacter, Neck);
            if (bodyPosition->state != NotTracked)
            {
                if (bodyPosition->cameraSpaceLocation.z < closestZ)
                {
                    closestZ = bodyPosition->cameraSpaceLocation.z;
                    closestCharacter = iCharacter;
                }
            }
        }
    }
    mActiveCharacter = closestCharacter;
    if (closestCharacter < 0) return; // no valid character found

    // calculate some useful joint values
    pgd::Vector torso = mKinect->GetJointData(closestCharacter, Neck)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, SpineBase)->cameraSpaceLocation;

    pgd::Vector rightThigh = mKinect->GetJointData(closestCharacter, KneeRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, HipRight)->cameraSpaceLocation;
    pgd::Vector rightShank = mKinect->GetJointData(closestCharacter, AnkleRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, KneeRight)->cameraSpaceLocation;
    pgd::Vector rightFoot = mKinect->GetJointData(closestCharacter, FootRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, AnkleRight)->cameraSpaceLocation;
    pgd::Vector rightArm = mKinect->GetJointData(closestCharacter, ElbowRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, ShoulderRight)->cameraSpaceLocation;
    pgd::Vector rightForearm = mKinect->GetJointData(closestCharacter, WristRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, ElbowRight)->cameraSpaceLocation;
    pgd::Vector rightHand = mKinect->GetJointData(closestCharacter, HandRight)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, WristRight)->cameraSpaceLocation;

    double rightHipAngle = 2*acos(pgd::FindRotation(rightThigh, torso).n);
    double rightKneeAngle = 2*acos(pgd::FindRotation(rightShank, rightThigh).n);
    double rightAnkleAngle = 2*acos(pgd::FindRotation(rightFoot, rightShank).n);
    double rightShoulderAngle = 2*acos(pgd::FindRotation(rightArm, torso).n);
    double rightElbowAngle = 2*acos(pgd::FindRotation(rightForearm, rightArm).n);
    double rightWristAngle = 2*acos(pgd::FindRotation(rightHand, rightForearm).n);

    pgd::Vector leftThigh = mKinect->GetJointData(closestCharacter, KneeLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, HipLeft)->cameraSpaceLocation;
    pgd::Vector leftShank = mKinect->GetJointData(closestCharacter, AnkleLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, KneeLeft)->cameraSpaceLocation;
    pgd::Vector leftFoot = mKinect->GetJointData(closestCharacter, FootLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, AnkleLeft)->cameraSpaceLocation;
    pgd::Vector leftArm = mKinect->GetJointData(closestCharacter, ElbowLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, ShoulderLeft)->cameraSpaceLocation;
    pgd::Vector leftForearm = mKinect->GetJointData(closestCharacter, WristLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, ElbowLeft)->cameraSpaceLocation;
    pgd::Vector leftHand = mKinect->GetJointData(closestCharacter, HandLeft)->cameraSpaceLocation - mKinect->GetJointData(closestCharacter, WristLeft)->cameraSpaceLocation;

    double leftHipAngle = 2*acos(pgd::FindRotation(leftThigh, torso).n);
    double leftKneeAngle = 2*acos(pgd::FindRotation(leftShank, leftThigh).n);
    double leftAnkleAngle = 2*acos(pgd::FindRotation(leftFoot, leftShank).n);
    double leftShoulderAngle = 2*acos(pgd::FindRotation(leftArm, torso).n);
    double leftElbowAngle = 2*acos(pgd::FindRotation(leftForearm, leftArm).n);
    double leftWristAngle = 2*acos(pgd::FindRotation(leftHand, leftForearm).n);

#if 0
    std::cerr << mSensorCount << " " << closestCharacter << " ";
    fprintf(stderr, "RH %5.2f RK %5.2f RA %5.2f RS %5.2f RE %5.2f RW %5.2f LH %5.2f LK %5.2f LA %5.2f LS %5.2f LE %5.2f LW %5.2f\n",
            rightHipAngle, rightKneeAngle, rightAnkleAngle, rightShoulderAngle, rightElbowAngle, rightWristAngle,
            leftHipAngle, leftKneeAngle, leftAnkleAngle, leftShoulderAngle, leftElbowAngle, leftWristAngle);
#endif

#if 0
    if (rightHipAngle > 2.4 )
    {
        mSimulationInterface->SetDriverValue("RightHipExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightHipFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightHipExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightHipFlexDriver", 1.0);
    }
    if (rightKneeAngle < 0.8 )
    {
        mSimulationInterface->SetDriverValue("RightKneeExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightKneeFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightKneeExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightKneeFlexDriver", 1.0);
    }
    if (rightAnkleAngle < 1.2 )
    {
        mSimulationInterface->SetDriverValue("RightAnkleExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightAnkleFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightAnkleExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightAnkleFlexDriver", 1.0);
    }
    if (rightShoulderAngle < 2.2 )
    {
        mSimulationInterface->SetDriverValue("RightShoulderExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightShoulderFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightShoulderExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightShoulderFlexDriver", 1.0);
    }
    if (rightElbowAngle < 0.7 )
    {
        mSimulationInterface->SetDriverValue("RightElbowExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightElbowFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightElbowExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightElbowFlexDriver", 1.0);
    }
    if (rightWristAngle < 0.2 )
    {
        mSimulationInterface->SetDriverValue("RightWristExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("RightWristFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("RightWristExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("RightWristFlexDriver", 1.0);
    }

    if (leftHipAngle > 2.4 )
    {
        mSimulationInterface->SetDriverValue("LeftHipExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftHipFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftHipExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftHipFlexDriver", 1.0);
    }
    if (leftKneeAngle < 0.8 )
    {
        mSimulationInterface->SetDriverValue("LeftKneeExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftKneeFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftKneeExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftKneeFlexDriver", 1.0);
    }
    if (leftAnkleAngle < 1.2 )
    {
        mSimulationInterface->SetDriverValue("LeftAnkleExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftAnkleFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftAnkleExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftAnkleFlexDriver", 1.0);
    }
    if (leftShoulderAngle < 2.2 )
    {
        mSimulationInterface->SetDriverValue("LeftShoulderExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftShoulderFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftShoulderExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftShoulderFlexDriver", 1.0);
    }
    if (leftElbowAngle < 0.7 )
    {
        mSimulationInterface->SetDriverValue("LeftElbowExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftElbowFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftElbowExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftElbowFlexDriver", 1.0);
    }
    if (leftWristAngle < 0.2 )
    {
        mSimulationInterface->SetDriverValue("LeftWristExtDriver", 1.0);
        mSimulationInterface->SetDriverValue("LeftWristFlexDriver", 0.0);
    }
    else
    {
        mSimulationInterface->SetDriverValue("LeftWristExtDriver", 0.0);
        mSimulationInterface->SetDriverValue("LeftWristFlexDriver", 1.0);
    }
#endif

    mAnatomicalCharacter->setKinectCharacter(mKinect->GetKinectCharacter(mActiveCharacter));
    mAnatomicalCharacter->UpdateAnatomicalJoints();

    UpdateJointAngleFrame();
#endif
}

void MainWindow::InitialiseJointAngleFrame()
{
    int i;
    QTableWidgetItem *tableWidgetItem;

    ui->tableWidgetJointAngles->setColumnCount(4);
    ui->tableWidgetJointAngles->setRowCount(24);

    tableWidgetItem = new QTableWidgetItem("Joint");
    ui->tableWidgetJointAngles->setHorizontalHeaderItem(0, tableWidgetItem);
    tableWidgetItem = new QTableWidgetItem("X");
    ui->tableWidgetJointAngles->setHorizontalHeaderItem(1, tableWidgetItem);
    tableWidgetItem = new QTableWidgetItem("Y");
    ui->tableWidgetJointAngles->setHorizontalHeaderItem(2, tableWidgetItem);
    tableWidgetItem = new QTableWidgetItem("Z");
    ui->tableWidgetJointAngles->setHorizontalHeaderItem(3, tableWidgetItem);

    for (i = 0; i < AnatomicalJointIDCount; i++)
    {
        tableWidgetItem = new QTableWidgetItem(ToString((AnatomicalJointID)i));
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 0, tableWidgetItem);
        tableWidgetItem = new QTableWidgetItem("0.0");
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 1, tableWidgetItem);
        tableWidgetItem = new QTableWidgetItem("0.0");
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 2, tableWidgetItem);
        tableWidgetItem = new QTableWidgetItem("0.0");
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 3, tableWidgetItem);
    }

    ui->tableWidgetJointAngles->resizeColumnToContents(0);

}


void MainWindow::UpdateJointAngleFrame()
{
    int i;
    QTableWidgetItem *tableWidgetItem;
    double x, y, z;

    if (mActiveCharacter < 0) // no active character so just zero everything
    {
        for (i = 0; i < AnatomicalJointIDCount; i++)
        {
            tableWidgetItem = new QTableWidgetItem("0.0");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetJointAngles->setItem(i, 1, tableWidgetItem);
            tableWidgetItem = new QTableWidgetItem("0.0");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetJointAngles->setItem(i, 2, tableWidgetItem);
            tableWidgetItem = new QTableWidgetItem("0.0");
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetJointAngles->setItem(i, 3, tableWidgetItem);
        }
    }

    for (i = 0; i < AnatomicalJointIDCount; i++)
    {
        x = mAnatomicalCharacter->GetAngle((AnatomicalJointID)i, 0);
        y = mAnatomicalCharacter->GetAngle((AnatomicalJointID)i, 1);
        z = mAnatomicalCharacter->GetAngle((AnatomicalJointID)i, 2);

        tableWidgetItem = new QTableWidgetItem(QString("%1").arg(x, 0, 'f', 1));
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 1, tableWidgetItem);
        tableWidgetItem = new QTableWidgetItem(QString("%1").arg(y, 0, 'f', 1));
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 2, tableWidgetItem);
        tableWidgetItem = new QTableWidgetItem(QString("%1").arg(z, 0, 'f', 1));
        tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetJointAngles->setItem(i, 3, tableWidgetItem);

        mkinectConnector->SetJointAngle((AnatomicalJointID)i, 0, x);
        mkinectConnector->SetJointAngle((AnatomicalJointID)i, 1, y);
        mkinectConnector->SetJointAngle((AnatomicalJointID)i, 2, z);
   }

    for (i = 0; i < AnatomicalJointIDCount; i++)
    {
        mkinectConnector->SetDrivablesAbsolute((AnatomicalJointID)i, 0, 0);
        mkinectConnector->SetDrivablesAbsolute((AnatomicalJointID)i, 1, 0);
        mkinectConnector->SetDrivablesAbsolute((AnatomicalJointID)i, 2, 0);
        mkinectConnector->UpdateDrivables((AnatomicalJointID)i, 0);
        mkinectConnector->UpdateDrivables((AnatomicalJointID)i, 1);
        mkinectConnector->UpdateDrivables((AnatomicalJointID)i, 2);
    }
}


void MainWindow::MenuRequestTableWidgetJointAngles(const QPoint &p)
{
    if (gSimulation == 0) return;

    int row = ui->tableWidgetJointAngles->currentRow();
    int col = ui->tableWidgetJointAngles->currentColumn();

    if (col < 1 || col > 4) return;

    QMenu menu(this);
    menu.addAction(tr("Set Link"));

    QPoint gp = ui->tableWidgetJointAngles->mapToGlobal(p);
    QAction *action = menu.exec(gp);

    if (action)
    {
        if (action->text() == tr("Set Link"))
        {
            // qDebug("Row selected %d, Col Selected %d\n", row, col);

            KinectConnectorDialog kinectConnectorDialog(this);
            KinectConnectorJoint *kinectConnectorJoint = mkinectConnector->GetKinectConnectorJoint((AnatomicalJointID)row, col - 1);
            kinectConnectorDialog.setValues(kinectConnectorJoint, gSimulation);
            kinectConnectorDialog.setWindowTitle(QString("Joint: %1 - %2").arg(ui->tableWidgetJointAngles->item(row, 0)->text()).arg(ui->tableWidgetJointAngles->item(row, col)->text()));
            int status = kinectConnectorDialog.exec();
            if (status == QDialog::Accepted)
            {
                kinectConnectorDialog.getValues(kinectConnectorJoint);
            }
        }
    }
}

