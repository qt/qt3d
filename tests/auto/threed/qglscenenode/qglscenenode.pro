TARGET = tst_qglscenenode
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d
CONFIG += warn_on

INCLUDEPATH += ../../../shared
SOURCES += tst_qglscenenode.cpp
QT+=widgets

CONFIG+=insignificant_test
