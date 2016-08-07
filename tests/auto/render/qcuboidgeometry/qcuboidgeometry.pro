TEMPLATE = app

TARGET = tst_qcuboidgeometry

QT += 3dextras testlib

CONFIG += testcase

SOURCES += \
    tst_qcuboidgeometry.cpp

include(../../core/common/common.pri)
include(../commons/commons.pri)
