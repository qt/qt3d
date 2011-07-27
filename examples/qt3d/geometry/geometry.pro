TEMPLATE = app
TARGET = geometry
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    geometry.rc

RC_FILE = geometry.rc

symbian {
    ICON = ../qt3d.svg
}
