TEMPLATE = app
TARGET = tea_service
QT += declarative widgets
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()

OTHER_FILES += \
    tea_service.rc

RC_FILE = tea_service.rc
