TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on
QT += 3d

include (../../../pkg.pri)

SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc

OTHER_FILES += \
    teaservice.rc

RC_FILE = teaservice.rc
