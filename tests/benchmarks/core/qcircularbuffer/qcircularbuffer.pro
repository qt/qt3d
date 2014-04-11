TARGET = tst_bench_qcircularbuffer
CONFIG += release
TEMPLATE = app
QT += testlib 3dcore

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += tst_bench_qcircularbuffer.cpp
