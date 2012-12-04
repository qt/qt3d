TEMPLATE=app
QT += testlib 3d
CONFIG += warn_on

SOURCES += tst_qglbuilder_perf.cpp
INCLUDEPATH += ../../../src/threed/geometry
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
