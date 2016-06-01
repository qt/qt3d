TEMPLATE = app

TARGET = tst_qkeyboardhandler

QT += core-private 3dcore 3dcore-private 3dinput 3dinput-private testlib

CONFIG += testcase

SOURCES += tst_qkeyboardhandler.cpp

include(../../render/commons/commons.pri)
include(../commons/commons.pri)
