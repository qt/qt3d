TARGET = tst_qglpickcolors
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on

INCLUDEPATH += ../../../../src/threed/painting
VPATH += ../../../../src/threed/painting

SOURCES += \
    tst_qglpickcolors.cpp \
    qglpickcolors.cpp

HEADERS += \
    qglpickcolors_p.h
