TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    teapot.rc

RC_FILE = teapot.rc

symbian {
    ICON = ../qt3d.svg
}
