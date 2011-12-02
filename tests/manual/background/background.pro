TEMPLATE = app
TARGET = tst_background
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()
