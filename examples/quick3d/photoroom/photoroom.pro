TEMPLATE = app
TARGET = photoroom
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml
include(../../../qt3dquick_pkg_dep.pri)
include(../../../pkg.pri)

symbian {
    qmlDeployment2.sources = qml\textures\*
    qmlDeployment2.path = qml\textures
    DEPLOYMENT += qmlDeployment2
}

OTHER_FILES += \
    photoroom.rc

RC_FILE = photoroom.rc
