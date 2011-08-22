TEMPLATE = app
TARGET = tst_basic
CONFIG += qt warn_on
QT += declarative

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()
