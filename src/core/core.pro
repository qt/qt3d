TARGET     = Qt3DCore
MODULE     = 3dcore

QT         = core-private gui-private

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(core.pri)

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

load(qt_module)
