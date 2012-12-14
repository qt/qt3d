TARGET = tst_qglsphere
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglsphere.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
