TEMPLATE = app
TARGET = matrix_animation
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)
include(../../../qml_pkg.pri)

OTHER_FILES += \
    matrix_animation.rc

RC_FILE = matrix_animation.rc
