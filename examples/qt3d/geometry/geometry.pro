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

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    geometry.rc

RC_FILE = geometry.rc
