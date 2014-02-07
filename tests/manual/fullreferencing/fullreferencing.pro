TEMPLATE = app
TARGET = tst_fullreferencing
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/TestFullReferencing.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/robot.3ds \
    qml/meshes/hazard.png

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
