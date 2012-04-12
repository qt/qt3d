TARGET = tst_qglpainter
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d
CONFIG += warn_on

INCLUDEPATH += ../../../shared
VPATH += ../../../shared

SOURCES += tst_qglpainter.cpp \
           qgltestwidget.cpp \
           compareimage.cpp \
           qglsimulator.cpp
HEADERS += qgltestwidget.h \
           compareimage.h \
           qglsimulator.h
