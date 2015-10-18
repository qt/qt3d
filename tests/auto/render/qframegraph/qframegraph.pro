TEMPLATE = app

TARGET = tst_qframegraph
QT += core-private 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_qframegraph.cpp

include(../commons/commons.pri)
