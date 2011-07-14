TEMPLATE = app
TARGET = photoroom
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)
include(../../../qml_pkg.pri)

OTHER_FILES += \
    photoroom.rc

RC_FILE = photoroom.rc
