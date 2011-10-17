TEMPLATE = app
TARGET = cube1
CONFIG += qt warn_on
QT += qt3d
SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
DESTDIR = $$QT.qt3d.bins/qt3d/tutorials
