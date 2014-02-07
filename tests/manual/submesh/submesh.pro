TEMPLATE = app
TARGET = tst_submesh

QT += 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/TestSubmesh.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/lunar-lander.3ds

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
