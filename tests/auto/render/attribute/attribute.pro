TEMPLATE = app

TARGET = tst_attribute

QT += 3dcore 3dcore-private 3drenderer 3drenderer-private testlib

CONFIG += testcase

SOURCES += \
    tst_attribute.cpp

include(../commons/commons.pri)
