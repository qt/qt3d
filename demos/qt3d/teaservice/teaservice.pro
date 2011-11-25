TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc

OTHER_FILES += \
    teaservice.rc \
    teaservice.desktop

RC_FILE = teaservice.rc

