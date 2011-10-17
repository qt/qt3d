TEMPLATE = app
TARGET = tst_model3ds
CONFIG += qt warn_on

QT += quick3d

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()
