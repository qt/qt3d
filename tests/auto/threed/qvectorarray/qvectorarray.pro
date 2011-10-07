TARGET = tst_qvectorarray
CONFIG += testcase
TEMPLATE=app
QT += testlib
QT += opengl

CONFIG += warn_on qt3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qvectorarray.cpp
