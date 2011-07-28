TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

OTHER_FILES += \
    teapot.rc

RC_FILE = teapot.rc
