load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on qt3d

INCLUDEPATH += ../../../shared
VPATH += ../../../shared

SOURCES += tst_qglpainter.cpp \
           qgltestwidget.cpp \
           compareimage.cpp \
           qglsimulator.cpp
HEADERS += qgltestwidget.h \
           compareimage.h \
           qglsimulator.h
QT+=widgets

CONFIG+=insignificant_test
