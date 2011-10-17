TARGET = tst_qglbuilder
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d
CONFIG += warn_on

INCLUDEPATH += ../../../shared
SOURCES += tst_qglbuilder.cpp
INCLUDEPATH += ../../../../src/threed/geometry
