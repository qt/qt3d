TEMPLATE = app
TARGET = solarsystem_qml
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    solarsystem_qml.rc \
    solarsystem_qml.desktop

RC_FILE = solarsystem_qml.rc
