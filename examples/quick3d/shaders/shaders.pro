TEMPLATE = app
TARGET = shaders
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Bouncing.qml \
    qml/Collapsing.qml \
    qml/Images.qml \
    qml/Interpolate.qml \
    qml/Shaders.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/basket.jpg \
    qml/qtlogo.png \
    qml/teapot-logo.png

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = examples
include(../../../pkg.pri)

#INSTALL_DIRS = qml
#mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    shaders.rc \
    $$QML_INFRA_FILES

RC_FILE = shaders.rc
