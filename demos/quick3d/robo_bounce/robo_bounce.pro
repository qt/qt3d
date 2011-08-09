TEMPLATE = app
TARGET = robo_bounce
QT += declarative
CONFIG += qt warn_on

SOURCES += main.cpp

include(../../../pkg.pri)

OTHER_FILES += \
    robo_bounce.rc

RC_FILE = robo_bounce.rc

RESOURCES += \
    robo_bounce.qrc

