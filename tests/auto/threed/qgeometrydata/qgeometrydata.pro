TARGET = tst_qgeometrydata
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d
CONFIG += warn_on

INCLUDEPATH += ../../../shared
INCLUDEPATH += ../../../../src/threed/geometry

SOURCES += tst_qgeometrydata.cpp
QT += widgets

CONFIG+=insignificant_test
