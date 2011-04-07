TEMPLATE = app
TARGET = builder
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
RESOURCES += builder.qrc

package {
    maemo {
        applnk.files = builder.desktop
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

