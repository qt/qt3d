TARGET = tst_qcircularbuffer
CONFIG += testcase
TEMPLATE = app
QT += testlib 3dcore

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += tst_qcircularbuffer.cpp
