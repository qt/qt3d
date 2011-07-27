TEMPLATE = app
TARGET = shaders
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)

symbian {
    qmlDeployment2.sources = qml\images\*
    qmlDeployment2.path = qml\images
    DEPLOYMENT += qmlDeployment2
    qmlDeployment3.sources = qml\meshes\*
    qmlDeployment3.path = qml\meshes
    DEPLOYMENT += qmlDeployment3
}

OTHER_FILES += \
    shaders.rc

RC_FILE = shaders.rc
