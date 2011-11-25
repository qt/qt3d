TEMPLATE = app
TARGET = geometry
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc

OTHER_FILES += \
    geometry.rc \
    geometry.desktop

RC_FILE = geometry.rc

