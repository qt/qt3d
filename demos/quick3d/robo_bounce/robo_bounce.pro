TEMPLATE = app
TARGET = robo_bounce
QT += qml quick 3dquick

SOURCES += main.cpp
ICON_FILE = ../icon.png

QML_FILES = \
    qml/Button.qml \
    qml/Calculator.qml \
    qml/desktop.qml \
    qml/Display.qml \
    qml/PushButton.qml \
    qml/QMLPanels.qml \
    qml/RoboBounce.qml \
    qml/Robot.qml \
    qml/TronCube.qml \

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/calculator.js \
    qml/qmldir

QML_MESHES_FILES = \
    qml/meshes/roboticHead.3ds \
    qml/meshes/button-red.png \
    qml/meshes/hazard.png \
    qml/meshes/infobutton.png \
    qml/meshes/roboticBody.3ds \
    qml/meshes/cubeButton.png \
    qml/meshes/roboticArms.3ds \
    qml/meshes/button-.png \
    qml/meshes/button-green.png \
    qml/meshes/roboInfo.png \
    qml/meshes/tronscape.png \
    qml/meshes/rotateButton2.png \
    qml/meshes/sQt3D_logo.png \
    qml/meshes/QML3DSign.png \
    qml/meshes/button-purple.png \
    qml/meshes/button.png \
    qml/meshes/display.png \
    qml/meshes/calculatorSign.png \
    qml/meshes/qtlogo.png \
    qml/meshes/button-blue.png

CATEGORY = demos
include(../../../pkg.pri)

RC_FILE = robo_bounce.rc

OTHER_FILES += \
    robo_bounce.rc \
    $$QML_INFRA_FILES \
    $$RC_FILE
