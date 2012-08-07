load(qt_build_config)

TARGET     = Qt3D
MODULE     = 3d
QT         = core-private gui-private
QT_PRIVATE = network

load(qt_module)

QMAKE_DOCS = $$PWD/doc/qt3d.qdocconf

gcov {
    CONFIG += static warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}

include(../private/private.pri)
include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
