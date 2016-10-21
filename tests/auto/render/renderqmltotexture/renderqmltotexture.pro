TEMPLATE = app

TARGET = tst_renderqmltotexture

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_renderqmltotexture.cpp

include(../../core/common/common.pri)
include(../commons/commons.pri)
