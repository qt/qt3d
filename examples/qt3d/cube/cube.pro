TEMPLATE = app
TARGET = cube
CONFIG += qt warn_on
QT += widgets qt3d

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    cube.rc

RC_FILE = cube.rc
