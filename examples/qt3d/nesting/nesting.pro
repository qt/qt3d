TEMPLATE = app
TARGET = nesting
QT += 3d

include(../../../pkg.pri)

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = nesting.qrc

OTHER_FILES += \
    nesting.rc

RC_FILE = nesting.rc
