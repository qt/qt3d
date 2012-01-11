TARGET = tst_qglvertexbundle
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d opengl
CONFIG += warn_on

SOURCES += tst_qglvertexbundle.cpp

macx:CONFIG+=insignificant_test
