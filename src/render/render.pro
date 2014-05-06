TARGET   = Qt3DRenderer

QT      += core-private gui-private qml 3dcore

DEFINES += QT3DRENDERER_LIBRARY

MODULE   = 3drenderer

load(qt_module)

include (backend/render-backend.pri)
include (frontend/render-frontend.pri)
include (io/render-io.pri)
include(../3rdparty/assimp/assimp.pri)


RESOURCES += $$PWD/render.qrc

OTHER_FILES += \
    $$PWD/shaders/*

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

HEADERS += $$PRIVATE_HEADERS \
    qt3drenderer_global.h \
    renderlogging.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

SOURCES += \
    renderlogging.cpp
