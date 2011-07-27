TEMPLATE = app
TARGET = nesting
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = nesting.qrc

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    nesting.rc

RC_FILE = nesting.rc

symbian {
    ICON = ../qt3d.svg
}
