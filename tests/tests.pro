TEMPLATE = subdirs

# load this since its needed to turn qtc_harmattan->maemo
include(../pkg.pri)

# we dont package unit tests for maemo
!maemo: SUBDIRS = manual
!package: SUBDIRS += auto benchmarks
