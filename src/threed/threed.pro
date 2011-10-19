load(qt_module)

TARGET     = Qt3D
MODULE     = qt3d
QT         = core-private gui-private opengl network widgets

CONFIG += module
MODULE_PRI = ../../modules/qt_qt3d.pri

load(qt_module_config)

gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}

include(../private/private.pri)
include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
