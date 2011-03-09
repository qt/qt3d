TEMPLATE = app
TARGET = builder
CONFIG += qt warn_on qt3d
SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
DESTDIR = ../../bin
RESOURCES += builder.qrc
