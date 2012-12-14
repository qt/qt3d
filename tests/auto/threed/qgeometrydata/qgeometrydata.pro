TARGET = tst_qgeometrydata
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
INCLUDEPATH += ../../../../src/threed/geometry

SOURCES += tst_qgeometrydata.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
