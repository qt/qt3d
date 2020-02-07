TARGET = tst_bench_jobs

TEMPLATE = app

QT += testlib core core-private 3dcore 3dcore-private 3drender 3drender-private 3dquick qml 3dextras

SOURCES += tst_bench_jobs.cpp

DEFINES += QT3D_RENDER_UNIT_TESTS
DEFINES += QT_BUILD_INTERNAL

# Link Against OpenGL Renderer Plugin
include(../../../auto/render/opengl/opengl_render_plugin.pri)
