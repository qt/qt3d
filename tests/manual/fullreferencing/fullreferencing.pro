TEMPLATE = app
TARGET = tst_fullreferencing
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()
