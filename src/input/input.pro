TARGET = Qt3DInput

QT += core core-private 3dcore 3dcore-private 3drenderer

DEFINES += QT3DINPUT_LIBRARY

MODULE = 3dinput

load(qt_module)

include(input.pri)

HEADERS += \
    qt3dinput_global.h
