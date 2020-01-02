TARGET = rhirenderer
QT += core-private gui-private 3dcore 3dcore-private 3drender 3drender-private shadertools shadertools-private
QT_PRIVATE = openglextensions

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

SOURCES += \
    main.cpp

DISTFILES += \
    rhirenderer.json

include(rhi.pri)

qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX2
}

qtConfig(qt3d-simd-sse2):!qtConfig(qt3d-simd-avx2) {
    CONFIG += simd
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_SSE2
}

PLUGIN_TYPE = renderers
PLUGIN_CLASS_NAME = RhiRendererPlugin
load(qt_plugin)
