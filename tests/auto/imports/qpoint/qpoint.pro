TARGET = tst_qpoint
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d 3dquick
QT += qml quick

SOURCES += tst_qpoint.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
