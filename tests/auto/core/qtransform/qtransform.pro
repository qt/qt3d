TARGET = tst_qtransform
CONFIG += testcase
TEMPLATE = app

SOURCES += tst_qtransform.cpp

QT += testlib 3dcore

include(../../render/commons/commons.pri)
