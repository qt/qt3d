TEMPLATE = app
TARGET = cube2
CONFIG += qt warn_on qt3d
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
DESTDIR = ../../../../bin/qt3d/tutorials

symbian {
    ICON = ../../qt3d.svg
}
