TEMPLATE = app
TARGET = penguin
CONFIG += qt warn_on qt3d
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model.qrc
DESTDIR = ../../../bin/qt3d/tutorials

symbian {    
    vendorinfo = \
     "%{\"Nokia\"}" \
     ":\"Nokia\""

    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment

    ICON = ../qt3d.svg
}
