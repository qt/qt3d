TEMPLATE = app
TARGET = geometry
QT += 3d

include(../../../pkg.pri)

SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc

OTHER_FILES += \
    geometry.rc

RC_FILE = geometry.rc
