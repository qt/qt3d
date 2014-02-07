TEMPLATE = app
TARGET = pageflip
QT += 3d

include(../../../pkg.pri)

SOURCES = pageflip.cpp pageflipmath.cpp
HEADERS = pageflipmath_p.h
RESOURCES = pageflip.qrc

OTHER_FILES += \
    pageflip.rc

RC_FILE = pageflip.rc
