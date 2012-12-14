TEMPLATE = app
TARGET = cylinder
QT += 3d

include(../../../pkg.pri)

SOURCES = cylinderview.cpp main.cpp
HEADERS = cylinderview.h
RESOURCES = cylinder.qrc

OTHER_FILES += \
    cylinder.rc

RC_FILE = cylinder.rc
