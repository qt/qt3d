TEMPLATE = app
TARGET = animations_qml
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/Animations.qml \
    qml/desktop.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/qtlogo.png \
    qml/stonewal.jpg \
    qml/cube_rotated.dae

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    animations_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = animations_qml.rc
