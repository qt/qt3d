TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on qt3d
SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc
DESTDIR = ../../bin
