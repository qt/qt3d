TARGET = tst_qray3d
CONFIG += testcase
TEMPLATE = app
QT += testlib 3d
CONFIG += warn_on
SOURCES += tst_qray3d.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
CONFIG+=insignificant_test  # See QTBUG-27189
