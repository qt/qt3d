TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = shapes.cpp

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    shapes.rc

RC_FILE = shapes.rc

symbian {
    ICON = ../qt3d.svg
}
