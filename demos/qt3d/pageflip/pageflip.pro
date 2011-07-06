TEMPLATE = app
TARGET = pageflip
CONFIG += qt warn_on

!package: CONFIG += qt3d

SOURCES = pageflip.cpp pageflipmath.cpp
HEADERS = pageflipmath_p.h
RESOURCES = pageflip.qrc

include(../../../qt3d_pkg_dep.pri)

OTHER_FILES += \
    pageflip.rc

RC_FILE = pageflip.rc

symbian {
    ICON = ../qt3d.svg
}
