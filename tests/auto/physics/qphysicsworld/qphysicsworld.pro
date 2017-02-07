TEMPLATE = app

TARGET = tst_qphysicsworld

QT += core-private 3dcore 3dcore-private 3dphysics 3dphysics-private testlib

CONFIG += testcase

SOURCES += tst_qphysicsworld.cpp

include(../../core/common/common.pri)
