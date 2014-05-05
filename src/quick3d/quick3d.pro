TARGET   = Qt3DQuick

QT      += core-private gui-private qml qml-private quick 3dcore

DEFINES += QT3DQUICK_LIBRARY

MODULE   = 3dquick

load(qt_module)

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

HEADERS += $$PRIVATE_HEADERS \
    qt3dquick_global.h \
    quickwindow.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

SOURCES += \
    quickwindow.cpp

