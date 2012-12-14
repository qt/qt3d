TARGET = tst_qglpickcolors
CONFIG += testcase
TEMPLATE=app
QT += testlib

INCLUDEPATH += ../../../../src/threed/painting
VPATH += ../../../../src/threed/painting

SOURCES += \
    tst_qglpickcolors.cpp \
    qglpickcolors.cpp

HEADERS += \
    qglpickcolors_p.h
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
