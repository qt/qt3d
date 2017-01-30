TEMPLATE = app

TARGET = tst_qlerpblend

QT += 3dcore 3dcore-private 3danimation 3danimation-private testlib

CONFIG += testcase

SOURCES += tst_qlerpblend.cpp

include(../../core/common/common.pri)
