TEMPLATE = app

TARGET = tst_qadditiveblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += \
    tst_qadditiveblend.cpp

include(../../core/common/common.pri)
