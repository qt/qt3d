TEMPLATE = app
TARGET = teapot_bounce_qml
CONFIG += qt warn_on
CONFIG += qt3dquick

SOURCES += main.cpp
HEADERS += ../qmlres.h

DESTDIR = ../../../bin

include(../../../qml_pkg.pri)
