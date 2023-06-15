/*
 *  KinectConnector.cpp
 *  GaitSymKinect
 *
 *  Created by Bill Sellers on 16/10/2015.
 *  Copyright 2015 Bill Sellers. All rights reserved.
 *
 */

#include "KinectConnector.h"
#include "FixedDriver.h"
#include "Simulation.h"

#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QDir>
#include <QFileDialog>

#include <map>
#include <string>

// Simulation globals
extern Simulation *gSimulation;

//
// KinectConnectorJoint
//
// This holds the information about the control output of individual kinect joints
//

KinectConnectorJoint::KinectConnectorJoint()
{
    m_jointAngle = 0;
}

KinectConnectorJoint::~KinectConnectorJoint()
{
    for (std::map<std::string, KinectConnectorParameters *>::const_iterator iter1 = m_parameterList.begin(); iter1 != m_parameterList.end(); iter1++) delete iter1->second;
}

void KinectConnectorJoint::SetDrivablesAbsolute(double newDrivableValue)
{
    for (std::map<std::string, KinectConnectorParameters *>::const_iterator iter1 = m_parameterList.begin(); iter1 != m_parameterList.end(); iter1++)
    {
        iter1->second->fixedDriver->SetValue(newDrivableValue);
    }
}


void KinectConnectorJoint::UpdateDrivables()
{
    double v;
    for (std::map<std::string, KinectConnectorParameters *>::const_iterator iter1 = m_parameterList.begin(); iter1 != m_parameterList.end(); iter1++)
    {
        v = (m_jointAngle - iter1->second->neutralAngle) * iter1->second->gain;
        if (v < iter1->second->lowerBound) v = iter1->second->lowerBound;
        else if (v > iter1->second->upperBound) v = iter1->second->upperBound;
        iter1->second->fixedDriver->AddValue(v);
    }
}

KinectConnectorParameters *KinectConnectorJoint::GetKinectConnectorParameter(const std::string driverName)
{
    std::map<std::string, KinectConnectorParameters *>::const_iterator foundIter = m_parameterList.find(driverName);
    if (foundIter == m_parameterList.end()) return 0;
    return foundIter->second;
}

void KinectConnectorJoint::SetKinectConnectorParameter(KinectConnectorParameters *kinectParameters)
{
    DeleteKinectConnectorParameter(*kinectParameters->fixedDriver->GetName());
    KinectConnectorParameters *newKinectParameters = new KinectConnectorParameters();
    *newKinectParameters = *kinectParameters;
    m_parameterList[*kinectParameters->fixedDriver->GetName()] = newKinectParameters;
}

void KinectConnectorJoint::DeleteKinectConnectorParameter(const std::string driverName)
{
    std::map<std::string, KinectConnectorParameters *>::iterator foundIter = m_parameterList.find(driverName);
    if (foundIter != m_parameterList.end())
    {
        delete foundIter->second;
        m_parameterList.erase(foundIter);
    }
}

//
// KinectConnector
//
// This holds the information about the control output of all the kinect joints
//

KinectConnector::KinectConnector()
{
    m_KinectConnectorJoints = new KinectConnectorJoint[AnatomicalJointIDCount * 3];
}

KinectConnector::~KinectConnector()
{
    delete [] m_KinectConnectorJoints;
}

void KinectConnector::SetJointAngle(AnatomicalJointID kinectJointId, int eulerAngleIndex, double angle)
{
    (m_KinectConnectorJoints + kinectJointId * 3 + eulerAngleIndex)->SetJointAngle(angle);
}

void KinectConnector::SetDrivablesAbsolute(AnatomicalJointID kinectJointId, int eulerAngleIndex, double value)
{
    (m_KinectConnectorJoints + kinectJointId * 3 + eulerAngleIndex)->SetDrivablesAbsolute(value);
}

void KinectConnector::UpdateDrivables(AnatomicalJointID kinectJointId, int eulerAngleIndex)
{
    (m_KinectConnectorJoints + kinectJointId * 3 + eulerAngleIndex)->UpdateDrivables();
}

KinectConnectorJoint *KinectConnector::GetKinectConnectorJoint(AnatomicalJointID kinectJointId, int eulerAngleIndex)
{
    return (m_KinectConnectorJoints + kinectJointId * 3 + eulerAngleIndex);
}

// this routine read the kinect data from the config file
int KinectConnector::Read(QString filename)
{
    QFileInfo filenameInfo(filename);
    QDir parentFolder = QDir(filenameInfo.path());

    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Unable to open config file: %s", qPrintable(filename));
        return __LINE__;
    }
    if (!doc.setContent(&file))
    {
        qWarning("Unable to parse config file: %s", qPrintable(filename));
        return __LINE__;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    // qWarning("Element name: %s", qPrintable(docElem.tagName()));
    if (docElem.tagName() != "GAITSYMODE") return __LINE__;
    QDomNode docChild = docElem.firstChild();
    while (!docChild.isNull())
    {
        QDomElement docElement = docChild.toElement(); // try to convert the node to an element.
        if (!docElement.isNull())
        {
            if (docElement.tagName() == "CONNECTOR")
            {
                QString typeAttribute = docElement.attribute("Type");
                if (typeAttribute == "Kinect")
                {
                    QDomNode kinectChild = docElement.firstChild();
                    while (!kinectChild.isNull())
                    {
                        QDomElement kinectElement = kinectChild.toElement(); // try to convert the node to an element.
                        if (!kinectElement.isNull())
                        {
                            if (kinectElement.tagName() == "KINECTJOINT")
                            {
                                QString kinectJointIDAttribute = kinectElement.attribute("JointID");
                                int kinectJointId = kinectElement.attribute("JointIndex").toInt();
                                int eulerAngleIndex = kinectElement.attribute("EulerAngleIndex").toInt();
                                KinectConnectorJoint *kinectConnectorJoint = GetKinectConnectorJoint((AnatomicalJointID)kinectJointId, eulerAngleIndex);

                                QDomNode kinectJointChild = kinectElement.firstChild();
                                while (!kinectJointChild.isNull())
                                {
                                    QDomElement kinectJointElement = kinectJointChild.toElement(); // try to convert the node to an element.
                                    if (!kinectJointElement.isNull())
                                    {
                                        if (kinectJointElement.tagName() == "KINECTDRIVER")
                                        {
                                            QString kinectDriverIDAttribute = kinectJointElement.attribute("DriverID");
                                            std::map<std::string, Driver *> *driverList = gSimulation->GetDriverList();
                                            FixedDriver *fixedDriver = dynamic_cast<FixedDriver *>(driverList->at(kinectDriverIDAttribute.toUtf8().constData()));
                                            if (fixedDriver == 0)
                                            {
                                                qWarning("Unable to find fixed driver: %s", qPrintable(kinectDriverIDAttribute));
                                                return __LINE__;
                                            }
                                            KinectConnectorParameters *kinectParameters = new KinectConnectorParameters();
                                            kinectParameters->fixedDriver = fixedDriver;
                                            kinectParameters->gain = kinectJointElement.attribute("Gain").toDouble();
                                            kinectParameters->neutralAngle = kinectJointElement.attribute("NeutralAngle").toDouble();
                                            kinectParameters->lowerBound = kinectJointElement.attribute("LowerBound").toDouble();
                                            kinectParameters->upperBound = kinectJointElement.attribute("UpperBound").toDouble();
                                            kinectConnectorJoint->SetKinectConnectorParameter(kinectParameters);
                                        }
                                    }
                                    kinectJointChild = kinectJointChild.nextSibling();
                                }

                            }
                        }
                        kinectChild = kinectChild.nextSibling();
                    }
                }
            }
        }
        docChild = docChild.nextSibling();
    }

    return 0;
}

int KinectConnector::Write(QString filename)
{
    int i, j;

    // we do one of two things. If filename exists we read it and change the CONNECTOR section
    // and if it does not exist we create a new file with a new CONNECTOR section

    QDomDocument doc;
    QDomElement docElem;
    QFile file(filename);
    if (file.exists() == false) // create a new file
    {
        docElem = doc.createElement("GAITSYMODE");
        doc.appendChild(docElem);
    }
    else // read existing file and delete the CONNECTOR
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("Unable to open config file: %s", qPrintable(filename));
            return __LINE__;
        }
        if (!doc.setContent(&file))
        {
            qWarning("Unable to parse config file: %s", qPrintable(filename));
            return __LINE__;
        }
        file.close();

        docElem = doc.documentElement();
        // qWarning("Element name: %s", qPrintable(docElem.tagName()));
        if (docElem.tagName() != "GAITSYMODE") return __LINE__;
        QDomNode docChild = docElem.firstChild();
        while (!docChild.isNull())
        {
            QDomElement docElement = docChild.toElement(); // try to convert the node to an element.
            if (!docElement.isNull())
            {
                if (docElement.tagName() == "CONNECTOR")
                {
                    QString typeAttribute = docElement.attribute("Type");
                    if (typeAttribute == "Kinect")
                    {
                        QDomNode oldChild = docChild;
                        docChild = docChild.nextSibling();
                        docElem.removeChild(oldChild);
                        continue;
                    }
                }
            }
            docChild = docChild.nextSibling();
        }
    }


    QDomElement connectorElement = doc.createElement("CONNECTOR");
    docElem.appendChild(connectorElement);

    connectorElement.setAttribute("Type", "Kinect");

    for (i = 0; i < AnatomicalJointIDCount; i++)
    {
        for (j = 0; j < 3; j++)
        {
            QDomElement kinectJointElement = doc.createElement("KINECTJOINT");
            connectorElement.appendChild(kinectJointElement);

            kinectJointElement.setAttribute("JointID", ToString((AnatomicalJointID)i));
            kinectJointElement.setAttribute("JointIndex", i);
            kinectJointElement.setAttribute("EulerAngleIndex", j);

            KinectConnectorJoint *kinectConnectorJoint = GetKinectConnectorJoint((AnatomicalJointID)i, j);
            std::map<std::string, Driver *> *driverList = gSimulation->GetDriverList();
            for (std::map<std::string, Driver *>::iterator iter = driverList->begin(); iter != driverList->end(); iter++)
            {
                FixedDriver *fixedDriver = dynamic_cast<FixedDriver *>(iter->second);
                if (fixedDriver)
                {
                    KinectConnectorParameters *kinectConnectorParameters = kinectConnectorJoint->GetKinectConnectorParameter(*fixedDriver->GetName());
                    if (kinectConnectorParameters)
                    {
                        QDomElement kinectDriverElement = doc.createElement("KINECTDRIVER");
                        kinectJointElement.appendChild(kinectDriverElement);

                        kinectDriverElement.setAttribute("DriverID", fixedDriver->GetName()->c_str());
                        kinectDriverElement.setAttribute("Gain", kinectConnectorParameters->gain);
                        kinectDriverElement.setAttribute("NeutralAngle", kinectConnectorParameters->neutralAngle);
                        kinectDriverElement.setAttribute("LowerBound", kinectConnectorParameters->lowerBound);
                        kinectDriverElement.setAttribute("UpperBound", kinectConnectorParameters->upperBound);
                    }
                }
            }
        }
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Unable to open to config file: %s", qPrintable(filename));
        return __LINE__;
    }

    // and now the actual xml doc
    QString xmlString = doc.toString();
    QByteArray xmlData = xmlString.toUtf8();
    qint64 bytesWritten = file.write(xmlData);
    if (bytesWritten != xmlData.size()) qWarning("Unable to write config file: %s", qPrintable(filename));
    file.close();

    return 0;
}
