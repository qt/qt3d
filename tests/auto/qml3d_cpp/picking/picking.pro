TARGET = tst_qml3d_cpp
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d 3dquick
QT += qml quick gui-private

SOURCES += tst_picking.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

OTHER_FILES += data/tst_picking.qml

TESTDATA = $$OTHER_FILES
