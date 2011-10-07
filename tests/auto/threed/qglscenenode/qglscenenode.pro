TARGET = tst_qglscenenode
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglscenenode.cpp
QT+=widgets

CONFIG+=insignificant_test
