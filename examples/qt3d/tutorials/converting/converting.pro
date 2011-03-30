TEMPLATE = app
TARGET = converting
CONFIG += qt warn_on
QT += opengl
SOURCES = converting.cpp
LIBS += -L../../../lib -L../../../bin

include(../../../src/threed/threed_dep.pri)
