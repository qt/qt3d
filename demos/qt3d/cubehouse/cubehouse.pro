TEMPLATE = app
TARGET = cubehouse
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = cubeview.cpp main.cpp projectivetextureeffect.cpp
HEADERS = cubeview.h projectivetextureeffect.h
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
RESOURCES = cube.qrc

OTHER_FILES += \
    shaders/objectlineartexgen.frag \
    shaders/objectlineartexgen.vert

OTHER_FILES += \
    cubehouse.rc

RC_FILE = cubehouse.rc

