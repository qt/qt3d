TARGET = tst_qglabstractsurface
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

SOURCES += tst_qglabstractsurface.cpp

CONFIG+=insignificant_test  # See QTBUG-25277
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
