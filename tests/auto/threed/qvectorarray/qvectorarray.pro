TARGET = tst_qvectorarray
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qvectorarray.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

linux:CONFIG += insignificant_test # QTBUG-37334
