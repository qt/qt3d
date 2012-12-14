TARGET = tank
TEMPLATE = app
QT += 3d

include(../../../pkg.pri)

SOURCES += main.cpp\
    tankview.cpp \
    tank.cpp \
    quadplane.cpp

HEADERS  += tankview.h \
    tank.h \
    quadplane.h

OTHER_FILES += \
    tank.rc

RC_FILE = tank.rc
