TARGET = tst_qglsection
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

INCLUDEPATH += ../../../shared
SOURCES += tst_qglsection.cpp
INCLUDEPATH += ../../../../src/threed/geometry
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
CONFIG+=insignificant_test  # See QTBUG-27189
