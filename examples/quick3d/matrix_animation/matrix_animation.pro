TEMPLATE = app
TARGET = matrix_animation
QT += qml quick 3dquick

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

OTHER_FILES += \
    matrix_animation.rc \
    $$QML_INFRA_FILES

RC_FILE = matrix_animation.rc
