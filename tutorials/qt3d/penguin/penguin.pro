TEMPLATE = app
TARGET = penguin
CONFIG += qt warn_on qt3d
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model.qrc
DESTDIR = ../../../../bin/qt3d/tutorials

symbian {    
    ICON = ../../qt3d.svg
}
