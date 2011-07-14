TEMPLATE = app
TARGET = robo_bounce
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)

OTHER_FILES += \
    robo_bounce.rc

RC_FILE = robo_bounce.rc

RESOURCES += \
    robo_bounce.qrc

