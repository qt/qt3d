load(qttest_p4.prf)
TEMPLATE = app
QT += testlib
CONFIG += unittest warn_on qt3d

INCLUDEPATH += ../../../../demos/shaderwizard/
VPATH += ../../../../demos/shaderwizard/

SOURCES += \
    tst_qglcolladafxeffectfactory.cpp

OTHER_FILES += collada_cube.xml
RESOURCES += tst_qglcolladafxeffectfactory.qrc
