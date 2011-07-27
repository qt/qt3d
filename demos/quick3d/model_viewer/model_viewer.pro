TEMPLATE = app
TARGET = model_viewer
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)

symbian {
    qmlDeployment2.sources = qml\meshes\*
    qmlDeployment2.path = qml\meshes
    DEPLOYMENT += qmlDeployment2
    qmlDeployment3.sources = qml\images\*
    qmlDeployment3.path = qml\images
    DEPLOYMENT += qmlDeployment3
}

OTHER_FILES += \
    model_viewer.rc

RC_FILE = model_viewer.rc
