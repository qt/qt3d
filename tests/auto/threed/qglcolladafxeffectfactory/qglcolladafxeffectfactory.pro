TARGET = tst_qglcolladafxeffectfactory
CONFIG += testcase
TEMPLATE = app
QT += testlib 3d

CONFIG += warn_on

INCLUDEPATH += ../../../../demos/shaderwizard/
VPATH += ../../../../demos/shaderwizard/

SOURCES += \
    tst_qglcolladafxeffectfactory.cpp

OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
