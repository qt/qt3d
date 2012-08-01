TEMPLATE=app
QT += testlib 3d
CONFIG += warn_on

SOURCES += tst_qglbuilder_perf.cpp
INCLUDEPATH += $$QT.3d.sources/geometry
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
