TEMPLATE = app
TARGET = teapot
QT += 3d

include(../../../pkg.pri)

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

OTHER_FILES += \
    teapot.rc

RC_FILE = teapot.rc
