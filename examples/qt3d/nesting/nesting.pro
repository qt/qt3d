TEMPLATE = app
TARGET = nesting
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = nesting.qrc

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    nesting.rc

RC_FILE = nesting.rc

symbian {
    ICON = ../qt3d.svg
}
