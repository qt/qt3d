TARGET = tst_qglcolladafxeffectfactory
CONFIG += testcase
TEMPLATE = app
QT += testlib
QT += opengl

CONFIG += warn_on qt3d

INCLUDEPATH += ../../../../demos/shaderwizard/
VPATH += ../../../../demos/shaderwizard/

SOURCES += \
    tst_qglcolladafxeffectfactory.cpp

OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
