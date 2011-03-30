TEMPLATE = app
TARGET = geometry
CONFIG += qt warn_on qt3d
SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc
DESTDIR = ../../bin
