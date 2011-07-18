TEMPLATE = app
TARGET = tst_rotation
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()
