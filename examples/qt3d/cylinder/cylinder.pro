TEMPLATE = app
TARGET = cylinder
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cylinderview.cpp main.cpp
HEADERS = cylinderview.h
RESOURCES = cylinder.qrc

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    cylinder.rc

RC_FILE = cylinder.rc

symbian {
    ICON = ../qt3d.svg
}
