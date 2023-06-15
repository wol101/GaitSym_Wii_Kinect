/*
 *  KinectConnectorDialog.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 16/10/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "KinectConnectorDialog.h"
#include "ui_KinectConnectorDialog.h"

#include "Simulation.h"
#include "Driver.h"
#include "FixedDriver.h"
#include "KinectConnector.h"

#include <QTableWidgetItem>
#include <QTableWidget>

#include <map>

KinectConnectorDialog::KinectConnectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KinectConnectorDialog)
{
    ui->setupUi(this);

    // connect(ui->tableWidgetFixedDrivers, SIGNAL(cellActivated(int, int)), this, SLOT(fixedDriverCellA(int, int)));
    connect(ui->tableWidgetFixedDrivers, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(fixedDriverCurrentCellChanged(int, int, int, int)));
}

KinectConnectorDialog::~KinectConnectorDialog()
{
    for (std::vector<KinectConnectorParameters *>::size_type s = 0; s < m_kinectConnectorParametersList.size(); s++) delete m_kinectConnectorParametersList[s];

    delete ui;
}

void KinectConnectorDialog::setValues(KinectConnectorJoint *kinectConnector, Simulation *simulation)
{
    QTableWidgetItem *tableWidgetItem;
    ui->tableWidgetFixedDrivers->setColumnCount(1);
    m_kinectConnectorParametersList.clear();

    // get all the required drivers
    std::map<std::string, Driver *> *driverList = simulation->GetDriverList();
    FixedDriver *fixedDriver;
    KinectConnectorParameters *kinectConnectorParameters;
    int row = 0;
    for (std::map<std::string, Driver *>::iterator iter = driverList->begin(); iter != driverList->end(); iter++)
    {
        fixedDriver = dynamic_cast<FixedDriver *>(iter->second);
        if (fixedDriver)
        {
            kinectConnectorParameters = new KinectConnectorParameters();
            kinectConnectorParameters->fixedDriver = fixedDriver;
            tableWidgetItem = new QTableWidgetItem(iter->first.c_str());
            tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
            KinectConnectorParameters *foundParameters = kinectConnector->GetKinectConnectorParameter(*fixedDriver->GetName());
            if (foundParameters == 0)
            {
                tableWidgetItem->setCheckState(Qt::Unchecked);
            }
            else
            {
                tableWidgetItem->setCheckState(Qt::Checked);
                kinectConnectorParameters->upperBound = foundParameters->upperBound;
                kinectConnectorParameters->lowerBound = foundParameters->lowerBound;
                kinectConnectorParameters->gain = foundParameters->gain;
                kinectConnectorParameters->neutralAngle = foundParameters->neutralAngle;
            }
            ui->tableWidgetFixedDrivers->setRowCount(row + 1);
            ui->tableWidgetFixedDrivers->setItem(row, 0, tableWidgetItem);
            m_kinectConnectorParametersList.push_back(kinectConnectorParameters);
            row++;
        }
    }
    ui->tableWidgetFixedDrivers->resizeColumnToContents(0);

    ui->lineEditGain->setText(QString("%1").arg(m_kinectConnectorParametersList[0]->gain));
    ui->lineEditLowerBound->setText(QString("%1").arg(m_kinectConnectorParametersList[0]->lowerBound));
    ui->lineEditNeutralAngle->setText(QString("%1").arg(m_kinectConnectorParametersList[0]->neutralAngle));
    ui->lineEditUpperBound->setText(QString("%1").arg(m_kinectConnectorParametersList[0]->upperBound));

}

void KinectConnectorDialog::getValues(KinectConnectorJoint *kinectConnector)
{
    int row = ui->tableWidgetFixedDrivers->currentRow();
    m_kinectConnectorParametersList[row]->gain = ui->lineEditGain->text().toDouble();
    m_kinectConnectorParametersList[row]->lowerBound = ui->lineEditLowerBound->text().toDouble();
    m_kinectConnectorParametersList[row]->neutralAngle = ui->lineEditNeutralAngle->text().toDouble();
    m_kinectConnectorParametersList[row]->upperBound = ui->lineEditUpperBound->text().toDouble();

    for (std::vector<KinectConnectorParameters *>::size_type i = 0; i < m_kinectConnectorParametersList.size(); i++)
    {
        if (ui->tableWidgetFixedDrivers->item(i, 0)->checkState() == Qt::Checked)
        {
            kinectConnector->SetKinectConnectorParameter(m_kinectConnectorParametersList[i]);
        }
        else
        {
            kinectConnector->DeleteKinectConnectorParameter(*m_kinectConnectorParametersList[i]->fixedDriver->GetName());
        }
    }
}

/*
void KinectConnectorDialog::fixedDriverCellClicked(int row, int column)
{
    if (m_lastRowSelected < 0) return;
    m_kinectConnectorParametersList[m_lastRowSelected]->gain = ui->lineEditGain->text().toDouble();
    m_kinectConnectorParametersList[m_lastRowSelected]->lowerBound = ui->lineEditLowerBound->text().toDouble();
    m_kinectConnectorParametersList[m_lastRowSelected]->neutralAngle = ui->lineEditNeutralAngle->text().toDouble();
    m_kinectConnectorParametersList[m_lastRowSelected]->upperBound = ui->lineEditUpperBound->text().toDouble();

    m_lastRowSelected = row;
    ui->lineEditGain->setText(QString("%1").arg(m_kinectConnectorParametersList[m_lastRowSelected]->gain));
    ui->lineEditLowerBound->setText(QString("%1").arg(m_kinectConnectorParametersList[m_lastRowSelected]->lowerBound));
    ui->lineEditNeutralAngle->setText(QString("%1").arg(m_kinectConnectorParametersList[m_lastRowSelected]->neutralAngle));
    ui->lineEditUpperBound->setText(QString("%1").arg(m_kinectConnectorParametersList[m_lastRowSelected]->upperBound));
}
*/

void KinectConnectorDialog::fixedDriverCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if (previousRow >= 0)
    {
        m_kinectConnectorParametersList[previousRow]->gain = ui->lineEditGain->text().toDouble();
        m_kinectConnectorParametersList[previousRow]->lowerBound = ui->lineEditLowerBound->text().toDouble();
        m_kinectConnectorParametersList[previousRow]->neutralAngle = ui->lineEditNeutralAngle->text().toDouble();
        m_kinectConnectorParametersList[previousRow]->upperBound = ui->lineEditUpperBound->text().toDouble();
    }

    ui->lineEditGain->setText(QString("%1").arg(m_kinectConnectorParametersList[currentRow]->gain));
    ui->lineEditLowerBound->setText(QString("%1").arg(m_kinectConnectorParametersList[currentRow]->lowerBound));
    ui->lineEditNeutralAngle->setText(QString("%1").arg(m_kinectConnectorParametersList[currentRow]->neutralAngle));
    ui->lineEditUpperBound->setText(QString("%1").arg(m_kinectConnectorParametersList[currentRow]->upperBound));
}

