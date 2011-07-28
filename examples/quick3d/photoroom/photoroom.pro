TEMPLATE = app
TARGET = photoroom
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

symbian {
    qmlDeployment2.sources = qml\textures\*
    qmlDeployment2.path = qml\textures
    DEPLOYMENT += qmlDeployment2
}

OTHER_FILES += \
    photoroom.rc

RC_FILE = photoroom.rc
