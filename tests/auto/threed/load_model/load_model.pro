TARGET = tst_load_model
CONFIG += testcase
TEMPLATE=app
QT += testlib 3d

SOURCES += tst_load_model.cpp

RESOURCES += \
    load_model.qrc
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
