TEMPLATE = app
TARGET = monkeygod
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)

symbian {
    qmlDeployment2.sources = qml\meshes\*
    qmlDeployment2.path = qml\meshes
    DEPLOYMENT += qmlDeployment2
}

OTHER_FILES += \
    monkeygod.rc

RC_FILE = monkeygod.rc
