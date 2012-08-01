TARGET = tst_qline
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d 3dquick
QT += qml quick
CONFIG += warn_on

SOURCES += tst_qline.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
