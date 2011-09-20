load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
QT += opengl
CONFIG += unittest warn_on qt3d
SOURCES += tst_qsphere3d.cpp
