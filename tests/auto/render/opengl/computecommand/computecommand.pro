TEMPLATE = app

TARGET = tst_computecommand

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_computecommand.cpp

include(../../../core/common/common.pri)
include(../../commons/commons.pri)

# Link Against OpenGL Renderer Plugin
include(../opengl_render_plugin.pri)
