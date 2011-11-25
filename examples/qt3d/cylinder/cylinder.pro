TEMPLATE = app
TARGET = cylinder
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = cylinderview.cpp main.cpp
HEADERS = cylinderview.h
RESOURCES = cylinder.qrc

OTHER_FILES += \
    cylinder.rc \
    cylinder.desktop

RC_FILE = cylinder.rc

