TARGET = Qt3DInput

QT += core core-private 3dcore 3dcore-private 3drender

DEFINES += QT3DINPUT_LIBRARY

MODULE = 3dinput

load(qt_module)

include(frontend/frontend.pri)
include(backend/backend.pri)

HEADERS += \
    qt3dinput_global.h \
    qt3dinput_global_p.h
