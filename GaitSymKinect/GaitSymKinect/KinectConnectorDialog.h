/*
 *  KinectConnectorDialog.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 16/10/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#ifndef KINECTCONNECTORDIALOG_H
#define KINECTCONNECTORDIALOG_H

#include <QDialog>

class KinectConnectorJoint;
struct KinectConnectorParameters;
class Simulation;

namespace Ui {
class KinectConnectorDialog;
}

class KinectConnectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KinectConnectorDialog(QWidget *parent = 0);
    ~KinectConnectorDialog();

    void setValues(KinectConnectorJoint *kinectConnector, Simulation *simulation);
    void getValues(KinectConnectorJoint *kinectConnector);

public slots:
    // void fixedDriverCellClicked(int row, int column);
    void fixedDriverCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);


private:
    Ui::KinectConnectorDialog *ui;

    std::vector<KinectConnectorParameters *> m_kinectConnectorParametersList;
};

#endif // KINECTCONNECTORDIALOG_H
