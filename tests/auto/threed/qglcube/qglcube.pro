TARGET = tst_qglcube
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglcube.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
