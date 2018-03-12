QT += core-private gui-private 3dcore 3dcore-private 3drender 3drender-private
QT_PRIVATE = openglextensions

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH
DEFINES += BUILD_QT3D_MODULE

# For Q_AUTOTEST_EXPORT to work
DEFINES += QT_BUILDING_QT

DISTFILES += \
    openglrenderer.json

include (renderer/renderer.pri)
include (jobs/jobs.pri)
include (io/io.pri)
include (textures/textures.pri)
include (graphicshelpers/graphicshelpers.pri)
include (managers/managers.pri)

!integrity: include (debug/debug.pri)

INCLUDEPATH += $$PWD

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2
}

qtConfig(qt3d-simd-sse2):!qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_SSE2
}
