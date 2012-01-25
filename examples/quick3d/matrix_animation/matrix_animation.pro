TEMPLATE = app
TARGET = matrix_animation
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/MatrixAnimation.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/qtlogo.png

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = examples
include(../../../pkg.pri)

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    matrix_animation.rc \
    $$QML_INFRA_FILES

RC_FILE = matrix_animation.rc
