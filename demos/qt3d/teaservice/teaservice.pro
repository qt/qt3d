TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on
QT += widgets

include(../../../pkg.pri)
QT += qt3d

SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    teaservice.rc

RC_FILE = teaservice.rc
