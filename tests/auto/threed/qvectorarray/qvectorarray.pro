TARGET = tst_qvectorarray
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

CONFIG += warn_on

INCLUDEPATH += ../../../shared
SOURCES += tst_qvectorarray.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
