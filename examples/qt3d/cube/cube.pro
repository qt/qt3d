TEMPLATE = app
TARGET = cube
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc

OTHER_FILES += \
    cube.rc

RC_FILE = cube.rc

