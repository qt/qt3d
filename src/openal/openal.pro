TARGET = Qt3DOpenAL

QT += core 3dcore

DEFINES += QT3DOPENAL_LIBRARY

MODULE = 3dopenal

load(qt_module)

include(openal.pri)

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

HEADERS += \
    qt3dopenal_global.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
