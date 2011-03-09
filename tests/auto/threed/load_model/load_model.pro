load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on qt3d

SOURCES += tst_load_model.cpp

RESOURCES += \
    load_model.qrc
