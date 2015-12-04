TEMPLATE = app

TARGET = tst_qlight

QT += core-private 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_qlight.cpp

include(../commons/commons.pri)
