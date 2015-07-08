TARGET   = Qt3DRenderer

QT      += core-private gui-private 3dcore 3dcore-private openglextensions concurrent

DEFINES += QT3DRENDERER_LIBRARY

MODULE   = 3drenderer

MODULE_PLUGIN_TYPES = \
    sceneparsers

load(qt_module)

QMAKE_DOCS = $$PWD/doc/qt3drenderer.qdocconf

include (backend/render-backend.pri)
include (frontend/render-frontend.pri)
include (io/render-io.pri)
include (defaults/defaults.pri)

RESOURCES += $$PWD/render.qrc

OTHER_FILES += \
    $$PWD/shaders/* \
    $$PWD/shaders/gl3/* \
    $$PWD/shaders/es2/*

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

HEADERS += \
    qt3drenderer_global.h \
    qt3drenderer_global_p.h \
    renderlogging_p.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

SOURCES += \
    renderlogging.cpp
