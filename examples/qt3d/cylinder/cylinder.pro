TEMPLATE = app
TARGET = cylinder
CONFIG += qt warn_on
QT += qt3d

include(../../../pkg.pri)

SOURCES = cylinderview.cpp main.cpp
HEADERS = cylinderview.h
RESOURCES = cylinder.qrc

OTHER_FILES += \
    cylinder.rc

RC_FILE = cylinder.rc
