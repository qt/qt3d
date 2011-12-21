TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on
QT += qt3d

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

OTHER_FILES += \
    teapot.rc

RC_FILE = teapot.rc
