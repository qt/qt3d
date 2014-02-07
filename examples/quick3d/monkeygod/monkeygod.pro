TEMPLATE = app
TARGET = monkeygod
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Monkeygod.qml \
    qml/Penguin.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/marble.png

QML_MESHES_FILES = \
    qml/meshes/monkey.3ds \
    qml/meshes/penguin.3ds

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    $$QML_INFRA_FILES \
    monkeygod.rc

RC_FILE = monkeygod.rc
