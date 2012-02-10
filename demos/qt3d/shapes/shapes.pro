TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on
QT += qt3d

include (../../../pkg.pri)

SOURCES = shapes.cpp

OTHER_FILES += \
    shapes.rc

RC_FILE = shapes.rc
