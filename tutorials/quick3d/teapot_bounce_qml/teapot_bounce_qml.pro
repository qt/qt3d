TEMPLATE = app
TARGET = teapot_bounce_qml
CONFIG += qt warn_on

QT += declarative quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/TeapotBounce.qml \
    qml/desktop.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = tutorials
include(../../../pkg.pri)

OTHER_FILES += \
    mt.qml \
    teapot_bounce_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = teapot_bounce_qml.rc
