TEMPLATE = subdirs
SUBDIRS = manual
include(../pkg.pri)
!package: SUBDIRS += auto benchmarks
