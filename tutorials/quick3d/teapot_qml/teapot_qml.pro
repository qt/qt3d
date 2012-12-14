TEMPLATE = app
TARGET = teapot_qml

QT += 3dquick quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Teapot.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = tutorials
include(../../../pkg.pri)

OTHER_FILES += \
    teapot_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = teapot_qml.rc
