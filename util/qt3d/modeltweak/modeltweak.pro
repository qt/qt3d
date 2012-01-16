TEMPLATE = app
TARGET = modeltweak
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp \
    quickfile.cpp

symbian {
    qmlDeployment2.sources = qml\meshes\*
    qmlDeployment2.path = qml\meshes
    DEPLOYMENT += qmlDeployment2
}

OTHER_FILES += \
    modeltweak.rc

RC_FILE = modeltweak.rc

HEADERS += \
    quickfile.h
