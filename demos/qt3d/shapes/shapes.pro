TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = shapes.cpp

OTHER_FILES += \
    shapes.rc \
    shapes.desktop

RC_FILE = shapes.rc
