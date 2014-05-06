TARGET   = Qt3DQuickRenderer

QT      += core qml quick 3dcore 3drenderer 3dquick

DEFINES += QT3DQUICKRENDERER_LIBRARY

MODULE   = 3dquickrenderer

load(qt_module)

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

HEADERS += $$PRIVATE_HEADERS \
    qt3dquickrenderer_global.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(./items/items.pri)

