TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on

include(../../../pkg.pri)
!package: CONFIG += qt3d

SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc

include(../../../qt3d_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    teaservice.rc

RC_FILE = teaservice.rc

symbian {
    ICON = ../qt3d.svg
}
