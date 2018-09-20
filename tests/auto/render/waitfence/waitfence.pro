TEMPLATE = app

TARGET = tst_waitfence

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_waitfence.cpp

include(../../core/common/common.pri)
include(../commons/commons.pri)
