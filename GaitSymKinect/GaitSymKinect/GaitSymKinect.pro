#-------------------------------------------------
#
# Project created by QtCreator 2014-06-12T17:12:03
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GaitSymKinect
TEMPLATE = app
DEFINES += dDOUBLE \
           USE_OPENGL \
           USE_QT \
           USE_WI_BB

macx {
ICON = images/Icon.icns

INCLUDEPATH += ../../GaitSymODE/src \
        /usr/include/libxml2 \
        ${HOME}/Unix/include

LIBS += -lxml2 \
        ${HOME}/Unix/lib/libode.a
}
else:win32 {
RC_FILE = app.rc

DEFINES += MALLOC_H_NEEDED \
        BYTE_ORDER=LITTLE_ENDIAN \
        NEED_BCOPY \
        LIBXML_STATIC \
        _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../../GaitSymODE/src \
        c:/Users/wis/Documents/Unix/include \
        c:/Users/wis/Documents/Unix/include/libxml2

LIBS += c:/Users/wis/Documents/Unix/lib/libxml2_a.lib \
        c:/Users/wis/Documents/Unix/lib/ode.lib \
        -L"C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x64" \
        -L"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib\amd64" \
        wsock32.lib ws2_32.lib

LIBS        += "C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\Lib\x64\kinect20.lib"
INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc"
}

SOURCES += main.cpp \
    MainWindow.cpp \
    Preferences.cpp \
    PreferencesDialog.cpp \
    GLWidget.cpp \
    ../../GaitSymODE/src/AMotorJoint.cpp \
    ../../GaitSymODE/src/BallJoint.cpp \
    ../../GaitSymODE/src/Body.cpp \
    ../../GaitSymODE/src/BoxCarDriver.cpp \
    ../../GaitSymODE/src/BoxGeom.cpp \
    ../../GaitSymODE/src/CappedCylinderGeom.cpp \
    ../../GaitSymODE/src/Contact.cpp \
    ../../GaitSymODE/src/Controller.cpp \
    ../../GaitSymODE/src/CyclicDriver.cpp \
    ../../GaitSymODE/src/CylinderWrapStrap.cpp \
    ../../GaitSymODE/src/DampedSpringMuscle.cpp \
    ../../GaitSymODE/src/DataFile.cpp \
    ../../GaitSymODE/src/DataTarget.cpp \
    ../../GaitSymODE/src/DataTargetQuaternion.cpp \
    ../../GaitSymODE/src/DataTargetScalar.cpp \
    ../../GaitSymODE/src/DataTargetVector.cpp \
    ../../GaitSymODE/src/Drivable.cpp \
    ../../GaitSymODE/src/Driver.cpp \
    ../../GaitSymODE/src/Environment.cpp \
    ../../GaitSymODE/src/ErrorHandler.cpp \
    ../../GaitSymODE/src/ExpressionFolder.cpp \
    ../../GaitSymODE/src/ExpressionFunTransform3D.cpp \
    ../../GaitSymODE/src/ExpressionMat.cpp \
    ../../GaitSymODE/src/ExpressionParser.cpp \
    ../../GaitSymODE/src/ExpressionRef.cpp \
    ../../GaitSymODE/src/ExpressionVar.cpp \
    ../../GaitSymODE/src/ExpressionVec.cpp \
    ../../GaitSymODE/src/Face.cpp \
    ../../GaitSymODE/src/FacetedBox.cpp \
    ../../GaitSymODE/src/FacetedCappedCylinder.cpp \
    ../../GaitSymODE/src/FacetedConicSegment.cpp \
    ../../GaitSymODE/src/FacetedObject.cpp \
    ../../GaitSymODE/src/FacetedPolyline.cpp \
    ../../GaitSymODE/src/FacetedSphere.cpp \
    ../../GaitSymODE/src/fec.cpp \
    ../../GaitSymODE/src/FixedJoint.cpp \
    ../../GaitSymODE/src/FloatingHingeJoint.cpp \
    ../../GaitSymODE/src/Geom.cpp \
    ../../GaitSymODE/src/GLUIRoutines.cpp \
    ../../GaitSymODE/src/GLUtils.cpp \
    ../../GaitSymODE/src/HingeJoint.cpp \
    ../../GaitSymODE/src/IrrlichtRoutines.cpp \
    ../../GaitSymODE/src/Joint.cpp \
    ../../GaitSymODE/src/MAMuscle.cpp \
    ../../GaitSymODE/src/MAMuscleComplete.cpp \
    ../../GaitSymODE/src/MAMuscleExtended.cpp \
    ../../GaitSymODE/src/Marker.cpp \
    ../../GaitSymODE/src/Muscle.cpp \
    ../../GaitSymODE/src/NamedObject.cpp \
    ../../GaitSymODE/src/NPointStrap.cpp \
    ../../GaitSymODE/src/ObjectiveMain.cpp \
    ../../GaitSymODE/src/OpenCLRoutines.cpp \
    ../../GaitSymODE/src/PCA.cpp \
    ../../GaitSymODE/src/PIDMuscleLength.cpp \
    ../../GaitSymODE/src/PIDTargetMatch.cpp \
    ../../GaitSymODE/src/PlaneGeom.cpp \
    ../../GaitSymODE/src/PositionReporter.cpp \
    ../../GaitSymODE/src/RayGeom.cpp \
    ../../GaitSymODE/src/Reporter.cpp \
    ../../GaitSymODE/src/Simulation.cpp \
    ../../GaitSymODE/src/SliderJoint.cpp \
    ../../GaitSymODE/src/SphereGeom.cpp \
    ../../GaitSymODE/src/StackedBoxCarDriver.cpp \
    ../../GaitSymODE/src/StepDriver.cpp \
    ../../GaitSymODE/src/Strap.cpp \
    ../../GaitSymODE/src/StrokeFont.cpp \
    ../../GaitSymODE/src/SwingClearanceAbortReporter.cpp \
    ../../GaitSymODE/src/TCP.cpp \
    ../../GaitSymODE/src/ThreePointStrap.cpp \
    ../../GaitSymODE/src/TIFFWrite.cpp \
    ../../GaitSymODE/src/TorqueReporter.cpp \
    ../../GaitSymODE/src/TrimeshGeom.cpp \
    ../../GaitSymODE/src/TwoCylinderWrapStrap.cpp \
    ../../GaitSymODE/src/TwoPointStrap.cpp \
    ../../GaitSymODE/src/UDP.cpp \
    ../../GaitSymODE/src/UGMMuscle.cpp \
    ../../GaitSymODE/src/UniversalJoint.cpp \
    ../../GaitSymODE/src/Util.cpp \
    ../../GaitSymODE/src/Warehouse.cpp \
    ../../GaitSymODE/src/XMLConverter.cpp \
    Trackball.cpp \
    ../../GaitSymODE/src/FixedDriver.cpp \
    SimulationInterface.cpp \
    KinectInterface.cpp \
    AboutDialog.cpp \
    KinectConnector.cpp \
    KinectConnectorDialog.cpp \
    AnatomicalCharacter.cpp

HEADERS += MainWindow.h \
    Preferences.h \
    PreferencesDialog.h \
    GLWidget.h \
    ../../GaitSymODE/src/AMotorJoint.h \
    ../../GaitSymODE/src/BallJoint.h \
    ../../GaitSymODE/src/Body.h \
    ../../GaitSymODE/src/BoxCarDriver.h \
    ../../GaitSymODE/src/BoxGeom.h \
    ../../GaitSymODE/src/CappedCylinderGeom.h \
    ../../GaitSymODE/src/Contact.h \
    ../../GaitSymODE/src/Controller.h \
    ../../GaitSymODE/src/CyclicDriver.h \
    ../../GaitSymODE/src/CylinderWrapStrap.h \
    ../../GaitSymODE/src/DampedSpringMuscle.h \
    ../../GaitSymODE/src/DataFile.h \
    ../../GaitSymODE/src/DataTarget.h \
    ../../GaitSymODE/src/DataTargetQuaternion.h \
    ../../GaitSymODE/src/DataTargetScalar.h \
    ../../GaitSymODE/src/DataTargetVector.h \
    ../../GaitSymODE/src/DebugControl.h \
    ../../GaitSymODE/src/Drivable.h \
    ../../GaitSymODE/src/Driver.h \
    ../../GaitSymODE/src/Environment.h \
    ../../GaitSymODE/src/ErrorHandler.h \
    ../../GaitSymODE/src/ExpressionFolder.h \
    ../../GaitSymODE/src/ExpressionFunTransform3D.h \
    ../../GaitSymODE/src/ExpressionMat.h \
    ../../GaitSymODE/src/ExpressionParser.h \
    ../../GaitSymODE/src/ExpressionRef.h \
    ../../GaitSymODE/src/ExpressionVar.h \
    ../../GaitSymODE/src/ExpressionVec.h \
    ../../GaitSymODE/src/Face.h \
    ../../GaitSymODE/src/FacetedBox.h \
    ../../GaitSymODE/src/FacetedCappedCylinder.h \
    ../../GaitSymODE/src/FacetedConicSegment.h \
    ../../GaitSymODE/src/FacetedObject.h \
    ../../GaitSymODE/src/FacetedPolyline.h \
    ../../GaitSymODE/src/FacetedSphere.h \
    ../../GaitSymODE/src/fec.h \
    ../../GaitSymODE/src/FixedJoint.h \
    ../../GaitSymODE/src/FloatingHingeJoint.h \
    ../../GaitSymODE/src/Geom.h \
    ../../GaitSymODE/src/GLUIRoutines.h \
    ../../GaitSymODE/src/GLUtils.h \
    ../../GaitSymODE/src/HingeJoint.h \
    ../../GaitSymODE/src/IrrlichtRoutines.h \
    ../../GaitSymODE/src/Joint.h \
    ../../GaitSymODE/src/MAMuscle.h \
    ../../GaitSymODE/src/MAMuscleComplete.h \
    ../../GaitSymODE/src/MAMuscleExtended.h \
    ../../GaitSymODE/src/Marker.h \
    ../../GaitSymODE/src/MPIStuff.h \
    ../../GaitSymODE/src/Muscle.h \
    ../../GaitSymODE/src/NamedObject.h \
    ../../GaitSymODE/src/NPointStrap.h \
    ../../GaitSymODE/src/ObjectiveMain.h \
    ../../GaitSymODE/src/OpenCLRoutines.h \
    ../../GaitSymODE/src/PCA.h \
    ../../GaitSymODE/src/PGDMath.h \
    ../../GaitSymODE/src/PIDMuscleLength.h \
    ../../GaitSymODE/src/PIDTargetMatch.h \
    ../../GaitSymODE/src/PlaneGeom.h \
    ../../GaitSymODE/src/PositionReporter.h \
    ../../GaitSymODE/src/RayGeom.h \
    ../../GaitSymODE/src/Reporter.h \
    ../../GaitSymODE/src/SimpleStrap.h \
    ../../GaitSymODE/src/Simulation.h \
    ../../GaitSymODE/src/SliderJoint.h \
    ../../GaitSymODE/src/SocketMessages.h \
    ../../GaitSymODE/src/SphereGeom.h \
    ../../GaitSymODE/src/StackedBoxCarDriver.h \
    ../../GaitSymODE/src/StepDriver.h \
    ../../GaitSymODE/src/Strap.h \
    ../../GaitSymODE/src/StrokeFont.h \
    ../../GaitSymODE/src/SwingClearanceAbortReporter.h \
    ../../GaitSymODE/src/TCP.h \
    ../../GaitSymODE/src/ThreePointStrap.h \
    ../../GaitSymODE/src/TIFFWrite.h \
    ../../GaitSymODE/src/TorqueReporter.h \
    ../../GaitSymODE/src/TrimeshGeom.h \
    ../../GaitSymODE/src/TwoCylinderWrapStrap.h \
    ../../GaitSymODE/src/TwoPointStrap.h \
    ../../GaitSymODE/src/UDP.h \
    ../../GaitSymODE/src/UGMMuscle.h \
    ../../GaitSymODE/src/UniversalJoint.h \
    ../../GaitSymODE/src/Util.h \
    ../../GaitSymODE/src/Warehouse.h \
    ../../GaitSymODE/src/XMLConverter.h \
    Trackball.h \
    ../../GaitSymODE/GaitSymQt/QTKitHelper.h \
    ../../GaitSymODE/src/FixedDriver.h \
    SimulationInterface.h \
    KinectInterface.h \
    AboutDialog.h \
    KinectConnector.h \
    KinectConnectorDialog.h \
    AnatomicalCharacter.h

FORMS += MainWindow.ui \
    PreferencesDialog.ui \
    AboutDialog.ui \
    KinectConnectorDialog.ui

RESOURCES += \
    resources.qrc

