TARGET = tst_qcapsulemesh
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d 3dquick
QT += qml quick

SOURCES += tst_qcapsulemesh.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
