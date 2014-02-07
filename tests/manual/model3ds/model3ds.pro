TEMPLATE = app
TARGET = tst_model3ds

QT += 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/TestModel3ds.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/hazard.png \
    qml/meshes/lunar-lander.3ds \
    qml/meshes/monkey.3ds \
    qml/meshes/penguin.3ds \
    qml/meshes/robot.3ds

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
