TARGET = tst_qglmaterial
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d
CONFIG += warn_on

SOURCES += tst_qglmaterial.cpp
macx: CONFIG+=insignificant_test
