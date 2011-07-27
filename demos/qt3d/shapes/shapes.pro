TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = shapes.cpp

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    shapes.rc

RC_FILE = shapes.rc

symbian {
    ICON = ../qt3d.svg
}
