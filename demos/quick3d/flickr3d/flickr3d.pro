TEMPLATE = app
TARGET = flickr3d
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    flickr3d.rc \
    flickr3d.desktop

RC_FILE = flickr3d.rc




