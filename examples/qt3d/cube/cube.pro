TEMPLATE = app
TARGET = cube
QT += 3d

include(../../../pkg.pri)

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc

OTHER_FILES += \
    cube.rc

RC_FILE = cube.rc
