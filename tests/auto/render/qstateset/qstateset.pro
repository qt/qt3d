TEMPLATE = app

TARGET = tst_qstateset
QT += core-private 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_qstateset.cpp

include(../commons/commons.pri)
