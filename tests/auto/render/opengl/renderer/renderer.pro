TEMPLATE = app

TARGET = tst_renderer

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_renderer.cpp

# Link Against OpenGL Renderer Plugin
include(../opengl_render_plugin.pri)

CONFIG += useCommonTestAspect
include(../../commons/commons.pri)
