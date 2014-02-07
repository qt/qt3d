TARGET = tst_qglcameraanimation
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../../threed/viewing
VPATH += ../../../../threed/viewing

SOURCES += tst_qglcameraanimation.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
