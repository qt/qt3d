TEMPLATE = app
TARGET = cubehouse
CONFIG += qt warn_on qt3d
SOURCES = cubeview.cpp main.cpp projectivetextureeffect.cpp
HEADERS = cubeview.h projectivetextureeffect.h
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
RESOURCES = cube.qrc
DESTDIR = ../../bin

OTHER_FILES += \
    shaders/objectlineartexgen.frag \
    shaders/objectlineartexgen.vert
