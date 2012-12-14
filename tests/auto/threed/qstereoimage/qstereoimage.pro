TARGET = tst_qstereoimage
CONFIG += testcase
TEMPLATE=app
QT += testlib

INCLUDEPATH += ../../../../examples/stereophoto
VPATH += ../../../../examples/stereophoto

HEADERS += qstereoimage.h
SOURCES += tst_qstereoimage.cpp qstereoimage.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
