TEMPLATE = app
TARGET = robo_bounce
CONFIG += qt warn_on

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

SOURCES += main.cpp

OTHER_FILES += \
    robo_bounce.rc

RC_FILE = robo_bounce.rc

RESOURCES += \
    robo_bounce.qrc


symbian {
    TARGET.EPOCHEAPSIZE = 0x40000 0x4000000
}
