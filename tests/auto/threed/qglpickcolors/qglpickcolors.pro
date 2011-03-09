load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../../src/threed/painting
VPATH += ../../../../src/threed/painting

SOURCES += \
    tst_qglpickcolors.cpp \
    qglpickcolors.cpp

HEADERS += \
    qglpickcolors_p.h
