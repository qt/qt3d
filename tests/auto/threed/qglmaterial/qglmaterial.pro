TARGET = tst_qglmaterial
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d
CONFIG += insignificant_test  # See QTBUG-25757

SOURCES += tst_qglmaterial.cpp
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
