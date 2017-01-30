TEMPLATE = app

TARGET = tst_qaddblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += tst_qaddblend.cpp

include(../../core/common/common.pri)
