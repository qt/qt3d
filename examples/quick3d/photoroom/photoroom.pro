TEMPLATE = app
TARGET = photoroom
CONFIG += qt warn_on
QT += declarative

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()

OTHER_FILES += \
    photoroom.rc

RC_FILE = photoroom.rc
