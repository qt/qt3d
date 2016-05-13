TARGET   = Qt3DExtras
MODULE   = 3dextras
QT      += core-private 3dcore 3dcore-private 3drender 3drender-private 3dinput 3dlogic

DEFINES += QT3DEXTRAS_LIBRARY

load(qt_module)

include (geometries/geometries.pri)
include (defaults/defaults.pri)

HEADERS += \
    qt3dextras_global.h

RESOURCES += $$PWD/extras.qrc

