TARGET = tst_qglattributedescription
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../../threed/viewing
VPATH += ../../../../threed/viewing

SOURCES += tst_qglattributedescription.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
