TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    moon.rc

RC_FILE = moon.rc

symbian {
    TARGET.EPOCHEAPSIZE = 40000 40000000
}
