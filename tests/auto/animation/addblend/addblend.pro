TEMPLATE = app

TARGET = tst_addblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += tst_addblend.cpp

include(../../core/common/common.pri)
