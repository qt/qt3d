TEMPLATE = app

TARGET = tst_qgraphicsutils

QT += 3dcore 3dcore-private 3drender 3drender-private testlib

CONFIG += testcase

SOURCES += tst_qgraphicsutils.cpp

# Link Against OpenGL Renderer Plugin
include(../opengl_render_plugin.pri)
