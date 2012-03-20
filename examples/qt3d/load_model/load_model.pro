TEMPLATE = app
TARGET = load_model
CONFIG += qt warn_on

QT += 3d

include(../../../pkg.pri)

SOURCES = load_model.cpp \
    load_model_main.cpp
HEADERS = load_model.h
RESOURCES += load_model.qrc

OTHER_FILES += \
    load_model.rc

RC_FILE = load_model.rc
