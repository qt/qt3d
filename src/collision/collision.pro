TARGET     = Qt3DCollision
MODULE     = 3dcollision
QT         = core-private gui-private 3dcore 3dcore-private

load(qt_module)

DEFINES += QT3DCOLLISION_LIBRARY

QMAKE_DOCS = $$PWD/doc/qt3dcollision.qdocconf

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(collision.pri)
