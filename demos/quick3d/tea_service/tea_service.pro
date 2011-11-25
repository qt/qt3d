TEMPLATE = app
TARGET = tea_service
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    tea_service.rc \
    tea_service.desktop

RC_FILE = tea_service.rc
