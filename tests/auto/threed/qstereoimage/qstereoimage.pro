load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on

INCLUDEPATH += ../../../../examples/stereophoto
VPATH += ../../../../examples/stereophoto

HEADERS += qstereoimage.h
SOURCES += tst_qstereoimage.cpp qstereoimage.cpp
