TARGET = Qt3DInput
MODULE = 3dinput

QT += core-private 3dcore-private

include(frontend/frontend.pri)
include(backend/backend.pri)

HEADERS += \
    qt3dinput_global.h \
    qt3dinput_global_p.h

load(qt_module)
