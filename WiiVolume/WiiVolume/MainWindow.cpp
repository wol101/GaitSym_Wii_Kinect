/*
 *  MainWindow.cpp
 *  WiiVolume
 *
 *  Created by Bill Sellers on 12/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
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

#include <QTimer>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QPalette>
#include <QKeyEvent>
#include <QResizeEvent>

#include <algorithm>
#include <iostream>

// Simulation globals
extern Simulation *gSimulation;
extern char *gConfigFilenamePtr;
extern char *gGraphicsRoot;
QSettings *settings = 0;

#ifdef USE_WIIUSE
#define WIIC_EVENT WIIUSE_EVENT
#define WIIC_STATUS WIIUSE_STATUS
#define WIIC_DISCONNECT WIIUSE_DISCONNECT
#define WIIC_UNEXPECTED_DISCONNECT WIIUSE_UNEXPECTED_DISCONNECT
#define WIIC_READ_DATA WIIUSE_READ_DATA
#define WIIC_NUNCHUK_INSERTED WIIUSE_NUNCHUK_INSERTED
#define WIIC_CLASSIC_CTRL_INSERTED WIIUSE_CLASSIC_CTRL_INSERTED
#define WIIC_GUITAR_HERO_3_CTRL_INSERTED WIIUSE_GUITAR_HERO_3_CTRL_INSERTED
#define WIIC_MOTION_PLUS_INSERTED WIIUSE_MOTION_PLUS_ACTIVATED
#define WIIC_BALANCE_BOARD_INSERTED WIIUSE_WII_BOARD_CTRL_INSERTED
#define WIIC_BALANCE_BOARD_REMOVED WIIUSE_WII_BOARD_CTRL_REMOVED
#define WIIC_NUNCHUK_REMOVED WIIUSE_NUNCHUK_REMOVED
#define WIIC_CLASSIC_CTRL_REMOVED WIIUSE_CLASSIC_CTRL_REMOVED
#define WIIC_GUITAR_HERO_3_CTRL_REMOVED WIIUSE_GUITAR_HERO_3_CTRL_REMOVED
#define WIIC_MOTION_PLUS_REMOVED WIIUSE_MOTION_PLUS_REMOVED
#define WIIC_USING_ACC WIIUSE_USING_ACC
#define WIIC_USING_MOTION_PLUS WIIUSE_USING_MOTION_PLUS
#define WIIC_USING_IR WIIUSE_USING_IR
#define wiic_init wiiuse_init
#define wiic_find wiiuse_find
#define wiic_connect wiiuse_connect
#define wiic_set_leds wiiuse_set_leds
#define wiic_status wiiuse_status
#define wiic_poll wiiuse_poll
#define EXP_BALANCE_BOARD EXP_WII_BOARD
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mWiiPollTimer = 0;
    mFound = 0;
    mConnected = 0;
    mBB = 0;
    mLastWeightIndex = 0;
    mWeightSum = 0;
    mWeightAverage = 0;
    mStepCount = 0;

    mSimulationInterface = new SimulationInterface();

    // put GLWidget into the Frame via a QGridLayout
    QGridLayout *gridLayout = new QGridLayout(ui->frameOpenGL);
    gridLayout->setMargin(1);
    gridLayout->setSpacing(1);

    mGLWidget = new GLWidget(this);
    gridLayout->addWidget(mGLWidget, 0, 0);
    SetDefaultLights(false);

    // we need the time so we can poll the wii motes
    mWiiPollTimer = new QTimer(this);
    connect(mWiiPollTimer, SIGNAL(timeout()), this, SLOT(PollWiiMotes()));

    // set up the process one thing timer
    mProcessOneThingTimer = new QTimer(this);
    connect(mProcessOneThingTimer, SIGNAL(timeout()), this, SLOT(ProcessOneThing()));

    // hook up the various button actions
    connect(ui->actionFind_Balance_Board, SIGNAL(triggered()), this, SLOT(FindWiimotes()));
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(SetPreferences()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenConfig()));
    connect(ui->actionWeight, SIGNAL(triggered()), this, SLOT(SetOutputWeight()));
    connect(ui->actionVolume, SIGNAL(triggered()), this, SLOT(SetOutputVolume()));
    connect(ui->actionLeft_Foot, SIGNAL(triggered()), this, SLOT(LeftFootButton()));
    connect(ui->actionRight_Foot, SIGNAL(triggered()), this, SLOT(RightFootButton()));
    connect(ui->actionRight_View, SIGNAL(triggered()), this, SLOT(RightViewButton()));
    connect(ui->actionFront_View, SIGNAL(triggered()), this, SLOT(FrontViewButton()));
    connect(ui->actionTop_View, SIGNAL(triggered()), this, SLOT(TopViewButton()));
    connect(ui->actionPreset_Centre_of_Interest, SIGNAL(triggered()), this, SLOT(PresetCOIButton()));
    connect(ui->actionFull_Screen, SIGNAL(triggered()), this, SLOT(FullScreenButton()));
    connect(ui->actionHide_Toolbar, SIGNAL(triggered()), this, SLOT(HideToolbarButton()));

    // read in the preferences file
    mPreferences = new Preferences();
    mPreferences->Read();
    if (mPreferences->MainWindowGeometry.size() > 0) restoreGeometry(mPreferences->MainWindowGeometry);
    if (mPreferences->MainWindowState.size() > 0) restoreState(mPreferences->MainWindowState);

    mGLWidget->SetCameraDistance(mPreferences->CameraDistance);
    mGLWidget->SetCameraFoV(mPreferences->CameraFoV);
    mGLWidget->SetCameraCOIX(mPreferences->CameraCOIX);
    mGLWidget->SetCameraCOIY(mPreferences->CameraCOIY);
    mGLWidget->SetCameraCOIZ(mPreferences->CameraCOIZ);
    mGLWidget->SetCameraVec(mPreferences->CameraVecX, mPreferences->CameraVecY, mPreferences->CameraVecZ);
    mGLWidget->SetCameraUp(mPreferences->CameraUpX, mPreferences->CameraUpY, mPreferences->CameraUpZ);
    mGLWidget->SetBackground(mPreferences->BackgroundColour.redF(), mPreferences->BackgroundColour.greenF(), mPreferences->BackgroundColour.blueF(), mPreferences->BackgroundColour.alphaF());

    mWeightList = new float[mPreferences->MovingAverageWindow];
    std::fill_n(mWeightList, mPreferences->MovingAverageWindow, 0);

    // and if AutoLoad is set, load up a new simulation
    if (mPreferences->AutoLoadFlag)
    {
        if (QFile(mPreferences->LastFileName).exists()) ReadConfig(mPreferences->LastFileName);
    }

    // and set up the interface
    switch (mPreferences->DisplayMode)
    {
    case 0:
        SetOutputWeight();
        break;
    case 1:
        SetOutputVolume();
        break;
    }
}

MainWindow::~MainWindow()
{
    delete mWeightList;
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
    // this is a horrible hack to make the settings values available to the simulator
    QSettings localSettings(mPreferences->Organisation, mPreferences->Application);
    settings = &localSettings;
    // create the simulation
    if (gSimulation) delete gSimulation;

    char *readFileName = new char[fileName.toUtf8().length() + 1];
    strcpy(readFileName, fileName.toUtf8());
    gConfigFilenamePtr = readFileName;

    char *graphicsRoot = new char[16];
    strcpy(graphicsRoot, "");
    gGraphicsRoot = graphicsRoot;

    QDir::setCurrent(QFileInfo(fileName).absolutePath());

    int err = ReadModel();
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
        bool trackingFlag = false;

        if ((mStepCount % mPreferences->FrameSkip) == 0)
        {
            if (trackingFlag)
            {
                Body *body = gSimulation->GetBody(gSimulation->GetInterface()->TrackBodyID.c_str());
                if (body)
                {
                    const double *position = dBodyGetPosition(body->GetBodyID());
                    mGLWidget->SetCameraCOIX(position[0]);
                }
            }
            mGLWidget->updateGL();
            //QString time = QString("%1").arg(gSimulation->GetTime(), 0, 'f', 5);
            //ui->lcdNumberTime->display(time);
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

void MainWindow::SetOutputWeight()
{
    mPreferences->DisplayMode = 0; // 0: weight, 1: volume
    ui->actionWeight->setEnabled(false);
    ui->actionVolume->setEnabled(true);
    QPixmap pix(":/images/kg_outline.png");
    ui->labelUnits->setPixmap(pix);
}

void MainWindow::SetOutputVolume()
{
    mPreferences->DisplayMode = 1; // 0: weight, 1: volume
    ui->actionWeight->setEnabled(true);
    ui->actionVolume->setEnabled(false);
    QPixmap pix(":/images/m3_outline.png");
    ui->labelUnits->setPixmap(pix);
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

void MainWindow::FindWiimotes()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->actionFind_Balance_Board->setEnabled(false);
    ui->statusBar->showMessage("Searching for Wiimotes");
    QApplication::processEvents();

    // initialise the wiimote data structures
    mWiimotes =  wiic_init(MAX_WIIMOTES);

    // search for some wiimotes
    int timeout = 5;
    mFound = wiic_find(mWiimotes, MAX_WIIMOTES, timeout);
    if (!mFound)
    {
        ui->statusBar->showMessage("No Wiimotes found");
    }
    else
    {
        ui->statusBar->showMessage(QString("%1 Wiimotes found").arg(mFound));
        mConnected = wiic_connect(mWiimotes, mFound);
        ui->statusBar->showMessage(QString("%1 Wiimotes connected").arg(mConnected));

        for (int i = 0; i < mConnected; i++)
        {
            wiic_set_leds(mWiimotes[i], WIIMOTE_LED_1);
            wiic_status(mWiimotes[i]);
        }

        mWiiPollTimer->start(mPreferences->SampleInterval);
    }

    ui->actionFind_Balance_Board->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::OutputValue()
{
    switch (mPreferences->DisplayMode)
    {
    case 0: // weight mode
        ui->lcdNumberOutput->display(QString("%1").arg(double(mWeightAverage), 5, 'f', 1, ' '));
        break;

    case 1: // volume mode
        ui->lcdNumberOutput->display(QString("%1").arg(double(mWeightAverage / mPreferences->BodyDensity), 5, 'f', 4, ' '));
        break;
    }
}

void MainWindow::PollWiiMotes()
{
    if (wiic_poll(mWiimotes, mConnected))
    {
        // This happens if something happened on any wiimote.
        // So go through each one and check if anything happened.
        for (int i = 0; i < MAX_WIIMOTES; ++i)
        {
            switch (mWiimotes[i]->event)
            {
            case WIIC_EVENT:
                // a generic event occured
                print_event(mWiimotes[i]);
                if (mWiimotes[i]->exp.type == EXP_BALANCE_BOARD)
                {
#ifdef USE_WIIC
                    mBB = (balance_board_t*)&mWiimotes[i]->exp.bb;
                    mWeightSum = mWeightSum + mBB->pressure_weight.weight - mWeightList[mLastWeightIndex];
                    mWeightList[mLastWeightIndex] = mBB->pressure_weight.weight;
                    mSimulationInterface->SetCornerWeights(mBB->pressure_weight.top_left, mBB->pressure_weight.top_right, mBB->pressure_weight.bottom_left, mBB->pressure_weight.bottom_right);
#else
#ifdef USE_WIIUSE
                    mBB = (wii_board_t*)&mWiimotes[i]->exp.wb;
                    float weight = mBB->tl + mBB->tr + mBB->bl + mBB->br;
                    mWeightSum = mWeightSum + weight - mWeightList[mLastWeightIndex];
                    mWeightList[mLastWeightIndex] = weight;
                    mSimulationInterface->SetCornerWeights(mBB->tl, mBB->tr, mBB->bl, mBB->br);
#endif
#endif
                    mWeightAverage = mWeightSum / mPreferences->MovingAverageWindow;
                    mLastWeightIndex++;
                    if (mLastWeightIndex >= mPreferences->MovingAverageWindow) mLastWeightIndex = 0;
                    // std::cerr << "mWeightSum=" << mWeightSum << " mWeightAverage=" << mWeightAverage << "\n";
                    OutputValue();
                }
                break;

            case WIIC_STATUS:
                // a status event occured
                break;

            case WIIC_DISCONNECT:
            case WIIC_UNEXPECTED_DISCONNECT:
                // the wiimote disconnected
                break;

            case WIIC_READ_DATA:
                // Data we requested to read was returned.
                //Take a look at mWiimotes[i]->read_req for the data.
                break;

            case WIIC_NUNCHUK_INSERTED:
            case WIIC_CLASSIC_CTRL_INSERTED:
            case WIIC_GUITAR_HERO_3_CTRL_INSERTED:
            case WIIC_MOTION_PLUS_INSERTED:
                // various items were inserted into the controller
                break;

            case WIIC_BALANCE_BOARD_INSERTED:
                printf("Balance Board connected.\n");
                break;

            case WIIC_BALANCE_BOARD_REMOVED:
                printf("Balance Board disconnected.\n");
                break;

            case WIIC_NUNCHUK_REMOVED:
            case WIIC_CLASSIC_CTRL_REMOVED:
            case WIIC_GUITAR_HERO_3_CTRL_REMOVED:
            case WIIC_MOTION_PLUS_REMOVED:
                // various items were removed from the controller
                break;

            default:
                break;
            }
        }
    }
}

void MainWindow::print_event(struct wiimote_t* wm)
{
    printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

    /* if a button is pressed, report it */
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))       printf("A pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))       printf("B pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))      printf("UP pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))    printf("DOWN pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))    printf("LEFT pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))   printf("RIGHT pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))   printf("MINUS pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))    printf("PLUS pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))     printf("ONE pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))     printf("TWO pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))    printf("HOME pressed\n");

    /* if the accelerometer is turned on then print angles */
    if (WIIC_USING_ACC(wm)) {
        printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
        printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
        printf("wiimote yaw   = %f\n", wm->orient.yaw);
    }

#ifdef USE_WIIC
    /* if the Motion Plus is turned on then print angle rates */
    if (WIIC_USING_MOTION_PLUS(wm)) {
        printf("motion plus roll rate = %f\n", wm->exp.mp.angle_rate_gyro.r);
        printf("motion plus pitch rate = %f\n", wm->exp.mp.angle_rate_gyro.p);
        printf("motion plus yaw rate = %f\n", wm->exp.mp.angle_rate_gyro.y);
    }
#endif

    /*
     *  If IR tracking is enabled then print the coordinates
     *  on the virtual screen that the wiimote is pointing to.
     *
     *  Also make sure that we see at least 1 dot.
     */
    if (WIIC_USING_IR(wm)) {
        int i = 0;

        /* go through each of the 4 possible IR sources */
        for (; i < 4; ++i) {
            /* check if the source is visible */
            if (wm->ir.dot[i].visible)
                printf("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
        }

        printf("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
        printf("IR z distance: %f\n", wm->ir.z);
    }

    /* show events specific to supported expansions */
    if (wm->exp.type == EXP_NUNCHUK) {
        /* nunchuk */
        struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

        if (IS_PRESSED(nc, NUNCHUK_BUTTON_C))       printf("Nunchuk: C pressed\n");
        if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z))       printf("Nunchuk: Z pressed\n");

        printf("nunchuk roll  = %f\n", nc->orient.roll);
        printf("nunchuk pitch = %f\n", nc->orient.pitch);
        printf("nunchuk yaw   = %f\n", nc->orient.yaw);

        printf("nunchuk joystick angle:     %f\n", nc->js.ang);
        printf("nunchuk joystick magnitude: %f\n", nc->js.mag);
    } else if (wm->exp.type == EXP_CLASSIC) {
        /* classic controller */
        struct classic_ctrl_t* cc = (classic_ctrl_t*)&wm->exp.classic;

        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZL))         printf("Classic: ZL pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_B))          printf("Classic: B pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_Y))          printf("Classic: Y pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_A))          printf("Classic: A pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_X))          printf("Classic: X pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_ZR))         printf("Classic: ZR pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_LEFT))       printf("Classic: LEFT pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_UP))         printf("Classic: UP pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_RIGHT))      printf("Classic: RIGHT pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_DOWN))       printf("Classic: DOWN pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_L))     printf("Classic: FULL L pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_MINUS))      printf("Classic: MINUS pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_HOME))       printf("Classic: HOME pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_PLUS))       printf("Classic: PLUS pressed\n");
        if (IS_PRESSED(cc, CLASSIC_CTRL_BUTTON_FULL_R))     printf("Classic: FULL R pressed\n");

        printf("classic L button pressed:         %f\n", cc->l_shoulder);
        printf("classic R button pressed:         %f\n", cc->r_shoulder);
        printf("classic left joystick angle:      %f\n", cc->ljs.ang);
        printf("classic left joystick magnitude:  %f\n", cc->ljs.mag);
        printf("classic right joystick angle:     %f\n", cc->rjs.ang);
        printf("classic right joystick magnitude: %f\n", cc->rjs.mag);
    } else if (wm->exp.type == EXP_GUITAR_HERO_3) {
        /* guitar hero 3 guitar */
        struct guitar_hero_3_t* gh3 = (guitar_hero_3_t*)&wm->exp.gh3;

        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_UP))     printf("Guitar: Strum Up pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_STRUM_DOWN))   printf("Guitar: Strum Down pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_YELLOW))       printf("Guitar: Yellow pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_GREEN))        printf("Guitar: Green pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_BLUE))         printf("Guitar: Blue pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_RED))          printf("Guitar: Red pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_ORANGE))       printf("Guitar: Orange pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_PLUS))         printf("Guitar: Plus pressed\n");
        if (IS_PRESSED(gh3, GUITAR_HERO_3_BUTTON_MINUS))        printf("Guitar: Minus pressed\n");

        printf("Guitar whammy bar:          %f\n", gh3->whammy_bar);
        printf("Guitar joystick angle:      %f\n", gh3->js.ang);
        printf("Guitar joystick magnitude:  %f\n", gh3->js.mag);
    } else if(wm->exp.type == EXP_BALANCE_BOARD) {
#ifdef USE_WIIC
        struct balance_board_t* bb = (balance_board_t*)&wm->exp.bb;

        printf("balance board top left weight: %f\n", bb->pressure_weight.top_left);
        printf("balance board top right weight: %f\n", bb->pressure_weight.top_right);
        printf("balance board bottom left weight: %f\n", bb->pressure_weight.bottom_left);
        printf("balance board bottom right weight: %f\n", bb->pressure_weight.bottom_right);
        printf("balance board total weight: %f\n", bb->pressure_weight.weight);
#else
#ifdef USE_WIIUSE
        struct wii_board_t* bb = (wii_board_t*)&wm->exp.wb;

        printf("balance board top left weight: %f\n", bb->tl);
        printf("balance board top right weight: %f\n", bb->tr);
        printf("balance board bottom left weight: %f\n", bb->bl);
        printf("balance board bottom right weight: %f\n", bb->br);

//        printf("balance board raw top left weight: %d\n", (int)bb->rtl);
//        printf("balance board raw top right weight: %d\n", (int)bb->rtr);
//        printf("balance board raw bottom left weight: %d\n", (int)bb->rbl);
//        printf("balance board raw bottom right weight: %d\n", (int)bb->rbr);

        printf("balance board total weight: %f\n", bb->tl + bb->tr + bb->bl + bb->br);
#endif
#endif
    }
}


