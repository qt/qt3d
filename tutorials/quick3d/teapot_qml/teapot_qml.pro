TEMPLATE = app
TARGET = teapot_qml
CONFIG += qt warn_on
CONFIG += qt3dquick


SOURCES += main.cpp
HEADERS += ../qmlres.h

DESTDIR = ../../../bin

include(../../../qml_pkg.pri)

symbian {
    vendorinfo = \
     "%{\"Nokia\"}" \
     ":\"Nokia\""

    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment

    ICON = ../qtquick3d.svg
}
