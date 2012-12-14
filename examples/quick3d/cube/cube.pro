TEMPLATE = app
TARGET = cube_qml
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Cube.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/qtlogo.png

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    cube_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = cube_qml.rc
