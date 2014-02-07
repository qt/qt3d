TARGET = tst_qglbuilder
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglbuilder.cpp
INCLUDEPATH += ../../../../src/threed/geometry
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
