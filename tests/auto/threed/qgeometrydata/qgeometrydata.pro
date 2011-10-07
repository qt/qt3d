TARGET = tst_qgeometrydata
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../shared
INCLUDEPATH += ../../../../src/threed/geometry

SOURCES += tst_qgeometrydata.cpp
QT += widgets

CONFIG+=insignificant_test
