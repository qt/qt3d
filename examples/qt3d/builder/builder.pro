TEMPLATE = app
TARGET = builder
CONFIG += qt warn_on

!package: CONFIG += qt3d

SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
RESOURCES += builder.qrc

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    builder.rc

RC_FILE = builder.rc
