TEMPLATE = app

TARGET = tst_additiveblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += \
    tst_additiveblend.cpp

include(../../core/common/common.pri)
