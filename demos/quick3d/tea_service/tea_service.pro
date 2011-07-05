TEMPLATE = app
TARGET = tea_service
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)
include(../../../qml_pkg.pri)

OTHER_FILES += \
    tea_service.rc

RC_FILE = tea_service.rc
