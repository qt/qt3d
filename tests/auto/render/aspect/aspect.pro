TEMPLATE = app

TARGET = tst_aspect

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_aspect.cpp

CONFIG += useCommonTestAspect
include(../commons/commons.pri)
