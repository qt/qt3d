TARGET = tank
TEMPLATE = app
CONFIG += qt warn_on
QT += widgets qt3d

SOURCES += main.cpp\
    tankview.cpp \
    tank.cpp \
    quadplane.cpp

HEADERS  += tankview.h \
    tank.h \
    quadplane.h

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    tank.rc

RC_FILE = tank.rc
