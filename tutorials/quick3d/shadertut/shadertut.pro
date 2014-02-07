TEMPLATE = app
TARGET = shadertut
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/shader-tutorial.qml \
    qml/desktop.qml \
    qml/TutorialTeapot.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = tutorials
include(../../../pkg.pri)

OTHER_FILES += \
    shadertut.rc \
    $$QML_INFRA_FILES

RC_FILE = shadertut.rc
