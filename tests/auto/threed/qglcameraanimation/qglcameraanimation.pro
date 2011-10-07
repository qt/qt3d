TARGET = tst_qglcameraanimation
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../../threed/viewing
VPATH += ../../../../threed/viewing

SOURCES += tst_qglcameraanimation.cpp
