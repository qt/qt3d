TEMPLATE = app
TARGET = positional_lighting
QT += qml quick 3d 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Maze.qml \
    qml/PositionalLighting.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/subdivided_cube.3ds \
    qml/meshes/grid.3ds

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    mt.qml \
    $$QML_INFRA_FILES \
    $$QML_MESHES_FILES \
    postitional_lighting.rc

RC_FILE = positional_lighting.rc

HEADERS +=
