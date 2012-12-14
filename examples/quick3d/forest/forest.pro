TEMPLATE = app
TARGET = forest_qml
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Forest.qml \
    qml/Tree.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/tree.png

QML_MESHES_FILES = \
    qml/meshes/ground.obj \
    qml/meshes/tree.obj

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    forest.rc \
    $$QML_INFRA_FILES

RC_FILE = forest.rc
