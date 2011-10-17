TARGET = tst_qglattributevalue
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d
CONFIG += warn_on

INCLUDEPATH += ../../../../threed/viewing
VPATH += ../../../../threed/viewing

SOURCES += tst_qglattributevalue.cpp
