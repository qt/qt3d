TEMPLATE = app
TARGET = cube
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = cube.qrc

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    cube.rc

RC_FILE = cube.rc

symbian {
    ICON = ../qt3d.svg
}
