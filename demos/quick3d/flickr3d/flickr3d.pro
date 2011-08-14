TEMPLATE = app
TARGET = flickr3d
QT += declarative
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()

OTHER_FILES += \
    flickr3d.rc

RC_FILE = flickr3d.rc

