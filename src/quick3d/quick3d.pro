load(qt_module)

TARGET     = Qt3DQuick
MODULE     = qt3dquick
QT         = core gui network declarative quick qt3d

CONFIG += module
MODULE_PRI = ../../modules/qt_qt3dquick.pri

load(qt_module_config)

gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}

include(quick3d.pri)

PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_QUICK_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
