TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on
QT += widgets qt3d

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    teapot.rc

RC_FILE = teapot.rc
