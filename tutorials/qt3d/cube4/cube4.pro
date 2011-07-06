TEMPLATE = app
TARGET = cube4
CONFIG += qt warn_on qt3d
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc
DESTDIR = ../../../../bin/qt3d/tutorials

symbian {
    ICON = ../../qt3d.svg
}
