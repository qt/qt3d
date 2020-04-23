TARGET   = Qt3DRender
MODULE   = 3drender

QT = core-private 3dcore-private
QT_FOR_PRIVATE = concurrent
DEFINES += BUILD_QT3D_MODULE

include (backend/render-backend.pri)
include (geometry/geometry.pri)
include (framegraph/framegraph.pri)
include (frontend/render-frontend.pri)
include (jobs/jobs.pri)
include (lights/lights.pri)
include (materialsystem/materialsystem.pri)
include (renderstates/renderstates.pri)
include (io/io.pri)
include (picking/picking.pri)
include (raycasting/raycasting.pri)
include (services/services.pri)
include (shadergraph/shadergraph.pri)
include (texture/texture.pri)
include (surfaces/surfaces.pri)

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

HEADERS += \
    renderlogging_p.h \
    qt3drender_global.h \
    qt3drender_global_p.h \
    aligned_malloc_p.h \
    qrendererplugin_p.h \
    qrendererpluginfactory_p.h

SOURCES += \
    renderlogging.cpp \
    qrendererplugin.cpp \
    qrendererpluginfactory.cpp

MODULE_PLUGIN_TYPES = \
    sceneparsers \
    geometryloaders \
    renderplugins \
    renderers

load(qt_module)
