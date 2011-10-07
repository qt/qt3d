TARGET = tst_qglsection
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglsection.cpp
INCLUDEPATH += ../../../../src/threed/geometry
