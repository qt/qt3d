TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

package {
    maemo {
        applnk.files = teapot.desktop
        applnk.path = /usr/share/applications

        icons.files = icon-l-qt3d.png
        icons.path = /usr/share/themes/base/meegotouch/icons
        INSTALLS += icons applnk
    }
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
