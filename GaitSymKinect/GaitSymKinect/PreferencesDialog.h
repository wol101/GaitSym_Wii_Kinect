/*
 *  PreferencesDialog.h.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 31/5/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "Preferences.h"

#include <QDialog>
#include <QColor>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void setValues(const Preferences &prefs);
    void getValues(Preferences *prefs);

    static QColor getIdealTextColor(const QColor& rBackgroundColor);
    static QColor getAlphaColorHint(const QColor& Color);

public slots:
    void backgroundColourClicked();
    void colourEnvironment();
    void colourBody();
    void colourJoint();
    void colourGeom();
    void colourGeomForce();
    void colourStrap();
    void colourStrapForce();
    void colourStrapCylinder();
    void colourReporter();

signals:
     void EmitBackgroundColour(float red, float green, float blue, float alpha);

private:
    Ui::PreferencesDialog *ui;

    QColor mBackgroundColour;
    QColor mEnvironmentColour;
    QColor mBodyColour;
    QColor mJointColour;
    QColor mGeomColour;
    QColor mGeomForceColour;
    QColor mStrapColour;
    QColor mStrapForceColour;
    QColor mStrapCylinderColour;
    QColor mReporterColour;

};

#endif // PREFERENCESDIALOG_H
