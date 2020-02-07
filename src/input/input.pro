TARGET = Qt3DInput

DEFINES += QT3DINPUT_LIBRARY
DEFINES += BUILD_QT3D_MODULE

MODULE = 3dinput
MODULE_PLUGIN_TYPES = 3dinputdevices

QT += core-private 3dcore-private

include(frontend/frontend.pri)
include(backend/backend.pri)

HEADERS += \
    qt3dinput_global.h \
    qt3dinput_global_p.h

load(qt_module)
