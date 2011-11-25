TEMPLATE = app
TARGET = cube_qml
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    cube_qml.rc \
    cube_qml.desktop

RC_FILE = cube_qml.rc
