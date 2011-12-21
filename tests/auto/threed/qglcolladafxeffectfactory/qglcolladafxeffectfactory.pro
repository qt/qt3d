TARGET = tst_qglcolladafxeffectfactory
CONFIG += testcase
TEMPLATE = app
QT += testlib qt3d

CONFIG += warn_on

INCLUDEPATH += ../../../../demos/shaderwizard/
VPATH += ../../../../demos/shaderwizard/

SOURCES += \
    tst_qglcolladafxeffectfactory.cpp

OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
