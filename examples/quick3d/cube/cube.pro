TEMPLATE = app
TARGET = cube_qml
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml
include(../../../qt3dquick_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    cube_qml.rc

RC_FILE = cube_qml.rc
