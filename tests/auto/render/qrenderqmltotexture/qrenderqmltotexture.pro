TEMPLATE = app

TARGET = qrenderqmltotexture

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_qrenderqmltotexture.cpp

include(../commons/commons.pri)
