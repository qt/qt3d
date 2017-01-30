TEMPLATE = app

TARGET = tst_lerpblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += tst_lerpblend.cpp

include(../../core/common/common.pri)
