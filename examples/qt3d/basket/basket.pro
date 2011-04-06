TEMPLATE = app
TARGET = basket
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = basketview.cpp main.cpp
HEADERS = basketview.h basket_data.h
RESOURCES = basket.qrc

package {
    maemo {
        applnk.files = basket.desktop
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
