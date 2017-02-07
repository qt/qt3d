TARGET = Qt3DPhysics

DEFINES += QT3DPHYSICS_LIBRARY

MODULE = 3dphysics

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

QT += core-private 3dcore-private

include(frontend/frontend.pri)
include(backend/backend.pri)

HEADERS += \
    qt3dphysics_global.h \
    qt3dphysics_global_p.h

load(qt_module)
