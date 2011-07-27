TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml
include(../../../qt3dquick_pkg_dep.pri)
include(../../../pkg.pri)

OTHER_FILES += \
    moon.rc

RC_FILE = moon.rc
