TEMPLATE = app

TARGET = tst_qconductedclipanimator

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += \
    tst_qconductedclipanimator.cpp

include(../../core/common/common.pri)
