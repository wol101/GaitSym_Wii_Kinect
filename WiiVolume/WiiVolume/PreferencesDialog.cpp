/*
 *  PreferencesDialog.cpp
 *  WiiVolume
 *
 *  Created by Bill Sellers on 12/06/2014.
 *  Copyright 2014 Bill Sellers. All rights reserved.
 *
 */

#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "Preferences.h"
#include "Simulation.h"
#include "Environment.h"
#include "Body.h"
#include "Joint.h"
#include "Geom.h"
#include "Muscle.h"
#include "CylinderWrapStrap.h"
#include "TwoCylinderWrapStrap.h"
#include "Contact.h"
#include "Reporter.h"

#include <QColorDialog>
#include <QAbstractSpinBox>

// Simulation globals
extern Simulation *gSimulation;

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonBackgroundColor, SIGNAL(clicked()), this, SLOT(backgroundColourClicked()));
    connect(ui->pushButtonEnvironmentColour, SIGNAL(clicked()), this, SLOT(colourEnvironment()));
    connect(ui->pushButtonBodyColour, SIGNAL(clicked()), this, SLOT(colourBody()));
    connect(ui->pushButtonJointColour, SIGNAL(clicked()), this, SLOT(colourJoint()));
    connect(ui->pushButtonGeomColour, SIGNAL(clicked()), this, SLOT(colourGeom()));
    connect(ui->pushButtonGeomForceColour, SIGNAL(clicked()), this, SLOT(colourGeomForce()));
    connect(ui->pushButtonStrapColour, SIGNAL(clicked()), this, SLOT(colourStrap()));
    connect(ui->pushButtonStrapForceColour, SIGNAL(clicked()), this, SLOT(colourStrapForce()));
    connect(ui->pushButtonStrapCylinderColour, SIGNAL(clicked()), this, SLOT(colourStrapCylinder()));
    connect(ui->pushButtonReporterColour, SIGNAL(clicked()), this, SLOT(colourReporter()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setValues(const Preferences &prefs)
{
    ui->lineEditBodyDensity->setText(QString("%1").arg(prefs.BodyDensity));
    ui->lineEditFrameSkip->setText(QString("%1").arg(prefs.FrameSkip));
    ui->lineEditEnvironmentX->setText(QString("%1").arg(prefs.EnvironmentX));
    ui->lineEditEnvironmentY->setText(QString("%1").arg(prefs.EnvironmentY));
    ui->lineEditEnvironmentZ->setText(QString("%1").arg(prefs.EnvironmentZ));
    ui->lineEditBodyX->setText(QString("%1").arg(prefs.BodyX));
    ui->lineEditBodyY->setText(QString("%1").arg(prefs.BodyY));
    ui->lineEditBodyZ->setText(QString("%1").arg(prefs.BodyZ));
    ui->lineEditJointX->setText(QString("%1").arg(prefs.JointX));
    ui->lineEditJointY->setText(QString("%1").arg(prefs.JointY));
    ui->lineEditJointZ->setText(QString("%1").arg(prefs.JointZ));
    ui->lineEditGeomX->setText(QString("%1").arg(prefs.GeomX));
    ui->lineEditGeomY->setText(QString("%1").arg(prefs.GeomY));
    ui->lineEditGeomZ->setText(QString("%1").arg(prefs.GeomZ));
    ui->lineEditGeomForceScale->setText(QString("%1").arg(prefs.GeomForceScale));
    ui->lineEditGeomForceRadius->setText(QString("%1").arg(prefs.GeomForceRadius));
    ui->lineEditStrapRadius->setText(QString("%1").arg(prefs.StrapRadius));
    ui->lineEditStrapForceRadius->setText(QString("%1").arg(prefs.StrapForceRadius));
    ui->lineEditStrapForceScale->setText(QString("%1").arg(prefs.StrapForceScale));
    ui->lineEditStrapCylinderLength->setText(QString("%1").arg(prefs.StrapCylinderLength));
    ui->lineEditReporterX->setText(QString("%1").arg(prefs.ReporterX));
    ui->lineEditReporterY->setText(QString("%1").arg(prefs.ReporterY));
    ui->lineEditReporterZ->setText(QString("%1").arg(prefs.ReporterZ));
    ui->lineEditPresetCOIX->setText(QString("%1").arg(prefs.PresetCOIX));
    ui->lineEditPresetCOIY->setText(QString("%1").arg(prefs.PresetCOIY));
    ui->lineEditPresetCOIZ->setText(QString("%1").arg(prefs.PresetCOIZ));

    ui->checkBoxAutoLoad->setChecked(prefs.AutoLoadFlag);

    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");
    mBackgroundColour = prefs.BackgroundColour;
    mEnvironmentColour = prefs.EnvironmentColour;
    mBodyColour = prefs.BodyColour;
    mJointColour = prefs.JointColour;
    mGeomColour = prefs.GeomColour;
    mGeomForceColour = prefs.GeomForceColour;
    mStrapColour = prefs.StrapColour;
    mStrapForceColour = prefs.StrapForceColour;
    mStrapCylinderColour = prefs.StrapCylinderColour;
    mReporterColour = prefs.ReporterColour;
    ui->pushButtonBackgroundColor->setStyleSheet(Color_STYLE.arg(mBackgroundColour.name()).arg(getIdealTextColor(mBackgroundColour).name()).arg(getAlphaColorHint(mBackgroundColour).name()));
    ui->pushButtonEnvironmentColour->setStyleSheet(Color_STYLE.arg(mEnvironmentColour.name()).arg(getIdealTextColor(mEnvironmentColour).name()).arg(getAlphaColorHint(mEnvironmentColour).name()));
    ui->pushButtonBodyColour->setStyleSheet(Color_STYLE.arg(mBodyColour.name()).arg(getIdealTextColor(mBodyColour).name()).arg(getAlphaColorHint(mBodyColour).name()));
    ui->pushButtonJointColour->setStyleSheet(Color_STYLE.arg(mJointColour.name()).arg(getIdealTextColor(mJointColour).name()).arg(getAlphaColorHint(mJointColour).name()));
    ui->pushButtonGeomColour->setStyleSheet(Color_STYLE.arg(mGeomColour.name()).arg(getIdealTextColor(mGeomColour).name()).arg(getAlphaColorHint(mGeomColour).name()));
    ui->pushButtonGeomForceColour->setStyleSheet(Color_STYLE.arg(mGeomForceColour.name()).arg(getIdealTextColor(mGeomForceColour).name()).arg(getAlphaColorHint(mGeomForceColour).name()));
    ui->pushButtonStrapColour->setStyleSheet(Color_STYLE.arg(mStrapColour.name()).arg(getIdealTextColor(mStrapColour).name()).arg(getAlphaColorHint(mStrapColour).name()));
    ui->pushButtonStrapForceColour->setStyleSheet(Color_STYLE.arg(mStrapForceColour.name()).arg(getIdealTextColor(mStrapForceColour).name()).arg(getAlphaColorHint(mStrapForceColour).name()));
    ui->pushButtonStrapCylinderColour->setStyleSheet(Color_STYLE.arg(mStrapCylinderColour.name()).arg(getIdealTextColor(mStrapCylinderColour).name()).arg(getAlphaColorHint(mStrapCylinderColour).name()));
    ui->pushButtonReporterColour->setStyleSheet(Color_STYLE.arg(mReporterColour.name()).arg(getIdealTextColor(mReporterColour).name()).arg(getAlphaColorHint(mReporterColour).name()));

}

void PreferencesDialog::getValues(Preferences *prefs)
{
    prefs->BodyDensity = ui->lineEditBodyDensity->text().toDouble();
    prefs->FrameSkip = ui->lineEditFrameSkip->text().toInt();
    prefs->EnvironmentX = ui->lineEditEnvironmentX->text().toDouble();
    prefs->EnvironmentY = ui->lineEditEnvironmentY->text().toDouble();
    prefs->EnvironmentZ = ui->lineEditEnvironmentZ->text().toDouble();
    prefs->BodyX = ui->lineEditBodyX->text().toDouble();
    prefs->BodyY = ui->lineEditBodyY->text().toDouble();
    prefs->BodyZ = ui->lineEditBodyZ->text().toDouble();
    prefs->JointX = ui->lineEditJointX->text().toDouble();
    prefs->JointY = ui->lineEditJointY->text().toDouble();
    prefs->JointZ = ui->lineEditJointZ->text().toDouble();
    prefs->GeomX = ui->lineEditGeomX->text().toDouble();
    prefs->GeomY = ui->lineEditGeomY->text().toDouble();
    prefs->GeomZ = ui->lineEditGeomZ->text().toDouble();
    prefs->GeomForceScale = ui->lineEditGeomForceScale->text().toDouble();
    prefs->GeomForceRadius = ui->lineEditGeomForceRadius->text().toDouble();
    prefs->StrapRadius = ui->lineEditStrapRadius->text().toDouble();
    prefs->StrapForceRadius = ui->lineEditStrapForceRadius->text().toDouble();
    prefs->StrapForceScale = ui->lineEditStrapForceScale->text().toDouble();
    prefs->StrapCylinderLength = ui->lineEditStrapCylinderLength->text().toDouble();
    prefs->ReporterX = ui->lineEditReporterX->text().toDouble();
    prefs->ReporterY = ui->lineEditReporterY->text().toDouble();
    prefs->ReporterZ = ui->lineEditReporterZ->text().toDouble();
    prefs->PresetCOIX = ui->lineEditPresetCOIX->text().toDouble();
    prefs->PresetCOIY = ui->lineEditPresetCOIY->text().toDouble();
    prefs->PresetCOIZ = ui->lineEditPresetCOIZ->text().toDouble();

    prefs->AutoLoadFlag = ui->checkBoxAutoLoad->isChecked();

    prefs->BackgroundColour = mBackgroundColour;
    prefs->EnvironmentColour = mEnvironmentColour;
    prefs->BodyColour = mBodyColour;
    prefs->JointColour = mJointColour;
    prefs->GeomColour = mGeomColour;
    prefs->GeomForceColour = mGeomForceColour;
    prefs->StrapColour = mStrapColour;
    prefs->StrapForceColour = mStrapForceColour;
    prefs->StrapCylinderColour = mStrapCylinderColour;
    prefs->ReporterColour = mReporterColour;
}

void PreferencesDialog::backgroundColourClicked()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor Color = QColorDialog::getColor(mBackgroundColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (Color.isValid())
    {
        ui->pushButtonBackgroundColor->setStyleSheet(Color_STYLE.arg(Color.name()).arg(getIdealTextColor(Color).name()).arg(getAlphaColorHint(Color).name()));
        mBackgroundColour = Color;
        emit EmitBackgroundColour(mBackgroundColour.redF(), mBackgroundColour.greenF(), mBackgroundColour.blueF(), mBackgroundColour.alphaF());
        parentWidget()->update();
    }
}

void PreferencesDialog::colourEnvironment()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mEnvironmentColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonEnvironmentColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mEnvironmentColour = colour;

        if (gSimulation)
        {
            gSimulation->GetEnvironment()->SetColour(mEnvironmentColour.redF(), mEnvironmentColour.greenF(), mEnvironmentColour.blueF(), mEnvironmentColour.alphaF());
            gSimulation->GetInterface()->EnvironmentColour.SetColour(mEnvironmentColour.redF(), mEnvironmentColour.greenF(), mEnvironmentColour.blueF(), mEnvironmentColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourBody()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mBodyColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonBodyColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mBodyColour = colour;

        if (gSimulation)
        {
            std::map<std::string, Body *> *bodyList = gSimulation->GetBodyList();
            std::map<std::string, Body *>::const_iterator bodyIter;
            for (bodyIter = bodyList->begin(); bodyIter != bodyList->end(); bodyIter++) bodyIter->second->SetColour(mBodyColour.redF(), mBodyColour.greenF(), mBodyColour.blueF(), mBodyColour.alphaF());
            gSimulation->GetInterface()->BodyColour.SetColour(mBodyColour.redF(), mBodyColour.greenF(), mBodyColour.blueF(), mBodyColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourJoint()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mJointColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonJointColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mJointColour = colour;

        if (gSimulation)
        {
            std::map<std::string, Joint *> *jointList = gSimulation->GetJointList();
            std::map<std::string, Joint *>::const_iterator jointIter;
            for (jointIter = jointList->begin(); jointIter != jointList->end(); jointIter++) jointIter->second->SetColour(mJointColour.redF(), mJointColour.greenF(), mJointColour.blueF(), mJointColour.alphaF());
            gSimulation->GetInterface()->JointColour.SetColour(mJointColour.redF(), mJointColour.greenF(), mJointColour.blueF(), mJointColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourGeom()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mGeomColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonGeomColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mGeomColour = colour;

        if (gSimulation)
        {
            std::map<std::string, Geom *> *geomList = gSimulation->GetGeomList();
            std::map<std::string, Geom *>::const_iterator geomIter;
            for (geomIter = geomList->begin(); geomIter != geomList->end(); geomIter++) geomIter->second->SetColour(mGeomColour.redF(), mGeomColour.greenF(), mGeomColour.blueF(), mGeomColour.alphaF());
            gSimulation->GetInterface()->GeomColour.SetColour(mGeomColour.redF(), mGeomColour.greenF(), mGeomColour.blueF(), mGeomColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourGeomForce()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mGeomForceColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonGeomForceColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mGeomForceColour = colour;

        if (gSimulation)
        {
            std::vector<Contact *> *contactList = gSimulation->GetContactList();
            for (unsigned int c = 0; c < contactList->size(); c++) (*contactList)[c]->SetColour(mGeomForceColour.redF(), mGeomForceColour.greenF(), mGeomForceColour.blueF(), mGeomForceColour.alphaF());
            gSimulation->GetInterface()->GeomForceColour.SetColour(mGeomColour.redF(), mGeomColour.greenF(), mGeomColour.blueF(), mGeomColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourStrap()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mStrapColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonStrapColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mStrapColour = colour;

        if (gSimulation)
        {
            std::map<std::string, Muscle *> *muscleList = gSimulation->GetMuscleList();
            std::map<std::string, Muscle *>::const_iterator muscleIter;
            for (muscleIter = muscleList->begin(); muscleIter != muscleList->end(); muscleIter++)
            {
                muscleIter->second->GetStrap()->SetColour(mStrapColour.redF(), mStrapColour.greenF(), mStrapColour.blueF(), mStrapColour.alphaF());
                muscleIter->second->GetStrap()->SetLastDrawTime(-1);
            }
            gSimulation->GetInterface()->StrapColour.SetColour(mStrapColour.redF(), mStrapColour.greenF(), mStrapColour.blueF(), mStrapColour.alphaF());
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourStrapForce()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mStrapForceColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonStrapForceColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mStrapForceColour = colour;

        if (gSimulation)
        {
            gSimulation->GetInterface()->StrapForceColour.SetColour(mStrapForceColour.redF(), mStrapForceColour.greenF(), mStrapForceColour.blueF(), mStrapForceColour.alphaF());
            std::map<std::string, Muscle *> *muscleList = gSimulation->GetMuscleList();
            std::map<std::string, Muscle *>::const_iterator muscleIter;
            for (muscleIter = muscleList->begin(); muscleIter != muscleList->end(); muscleIter++)
            {
                muscleIter->second->GetStrap()->SetForceColour(gSimulation->GetInterface()->StrapForceColour);
                muscleIter->second->GetStrap()->SetLastDrawTime(-1);
            }
            parentWidget()->update();
        }
    }
}


void PreferencesDialog::colourStrapCylinder()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mStrapCylinderColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonStrapCylinderColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mStrapCylinderColour = colour;

        if (gSimulation)
        {
            gSimulation->GetInterface()->StrapCylinderColour.SetColour(mStrapCylinderColour.redF(), mStrapCylinderColour.greenF(), mStrapCylinderColour.blueF(), mStrapCylinderColour.alphaF());
            std::map<std::string, Muscle *> *muscleList = gSimulation->GetMuscleList();
            std::map<std::string, Muscle *>::const_iterator muscleIter;
            for (muscleIter = muscleList->begin(); muscleIter != muscleList->end(); muscleIter++)
            {
                CylinderWrapStrap *cylinderWrapStrap = dynamic_cast<CylinderWrapStrap *>(muscleIter->second->GetStrap());
                if (cylinderWrapStrap)
                {
                    cylinderWrapStrap->SetCylinderColour(gSimulation->GetInterface()->StrapCylinderColour);
                    cylinderWrapStrap->SetLastDrawTime(-1);
                }
                else
                {
                    TwoCylinderWrapStrap *twoCylinderWrapStrap = dynamic_cast<TwoCylinderWrapStrap *>(muscleIter->second->GetStrap());
                    if (twoCylinderWrapStrap)
                    {
                        twoCylinderWrapStrap->SetCylinderColour(gSimulation->GetInterface()->StrapCylinderColour);
                        twoCylinderWrapStrap->SetLastDrawTime(-1);
                    }
                }
            }
            parentWidget()->update();
        }
    }
}

void PreferencesDialog::colourReporter()
{
    const QString Color_STYLE("QPushButton { background-color : %1; color : %2; border: 4px solid %3; }");

    QColor colour;
    colour = QColorDialog::getColor(mReporterColour, this, "Select Color", QColorDialog::ShowAlphaChannel /* | QColorDialog::DontUseNativeDialog */);
    if (colour.isValid())
    {
        ui->pushButtonReporterColour->setStyleSheet(Color_STYLE.arg(colour.name()).arg(getIdealTextColor(colour).name()).arg(getAlphaColorHint(colour).name()));
        mReporterColour = colour;

        if (gSimulation)
        {
            std::map<std::string, Reporter *> *reporterList = gSimulation->GetReporterList();
            std::map<std::string, Reporter *>::const_iterator reporterIter;
            for (reporterIter = reporterList->begin(); reporterIter != reporterList->end(); reporterIter++) reporterIter->second->SetColour(mReporterColour.redF(), mReporterColour.greenF(), mReporterColour.blueF(), mReporterColour.alphaF());
            gSimulation->GetInterface()->ReporterColour.SetColour(mReporterColour.redF(), mReporterColour.greenF(), mReporterColour.blueF(), mReporterColour.alphaF());
            parentWidget()->update();
        }
    }
}

// return an ideal label Color, based on the given background Color.
// Based on http://www.codeproject.com/cs/media/IdealTextColor.asp
QColor PreferencesDialog::getIdealTextColor(const QColor& rBackgroundColor)
{
    const int THRESHOLD = 105;
    int BackgroundDelta = (rBackgroundColor.red() * 0.299) + (rBackgroundColor.green() * 0.587) + (rBackgroundColor.blue() * 0.114);
    return QColor((255- BackgroundDelta < THRESHOLD) ? Qt::black : Qt::white);
}

QColor PreferencesDialog::getAlphaColorHint(const QColor& Color)
{
    // (source * Blend.SourceAlpha) + (background * Blend.InvSourceAlpha)
    QColor background;
    background.setRgbF(1.0, 1.0, 1.0);
    QColor hint;
    hint.setRedF((Color.redF() * Color.alphaF()) + (background.redF() * (1 - Color.alphaF())));
    hint.setGreenF((Color.greenF() * Color.alphaF()) + (background.greenF() * (1 - Color.alphaF())));
    hint.setBlueF((Color.blueF() * Color.alphaF()) + (background.blueF() * (1 - Color.alphaF())));
    return hint;
}




