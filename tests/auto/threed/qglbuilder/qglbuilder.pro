TARGET = tst_qglbuilder
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglbuilder.cpp
INCLUDEPATH += ../../../../src/threed/geometry
