TEMPLATE = app
TARGET = builder
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
RESOURCES += builder.qrc

OTHER_FILES += \
    builder.rc

RC_FILE = builder.rc
