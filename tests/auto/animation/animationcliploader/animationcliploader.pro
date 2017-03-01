TEMPLATE = app

TARGET = tst_animationcliploader

QT += core-private 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += \
    tst_animationcliploader.cpp

include(../../core/common/common.pri)
