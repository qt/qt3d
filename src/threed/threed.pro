TARGET     = Qt3D
MODULE     = 3d
QT         = core-private gui-private opengl
QT_PRIVATE = network

load(qt_module)

QMAKE_DOCS = $$PWD/doc/qt3d.qdocconf

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(../private/private.pri)
include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
