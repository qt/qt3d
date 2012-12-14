TARGET = tst_qspheremesh
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d 3dquick
QT += qml quick

SOURCES += tst_qspheremesh.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
