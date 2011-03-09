CONFIG += qt warn_on qt3d
TARGET = tank
TEMPLATE = app

SOURCES += main.cpp\
    tankview.cpp \
    tank.cpp \
    quadplane.cpp

HEADERS  += tankview.h \
    tank.h \
    quadplane.h

DESTDIR = ../../bin
