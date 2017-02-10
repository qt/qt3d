TEMPLATE = app

TARGET = tst_qabstractclipblendnode

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += tst_qabstractclipblendnode.cpp

include(../../core/common/common.pri)
