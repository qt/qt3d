TARGET = Qt3DAnimation

DEFINES += QT3DANIMATION_LIBRARY
DEFINES += BUILD_QT3D_MODULE

MODULE = 3danimation

QT += core-private 3dcore-private 3drender 3drender-private

include(frontend/frontend.pri)
include(backend/backend.pri)

HEADERS += \
    qt3danimation_global.h \
    qt3danimation_global_p.h \
    animationlogging_p.h \
    job_common_p.h

SOURCES += \
    animationlogging.cpp

load(qt_module)
