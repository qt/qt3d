TEMPLATE = app
TARGET = teaservice
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = teaservice.cpp meshobject.cpp sceneobject.cpp perpixeleffect.cpp
HEADERS = meshobject.h sceneobject.h perpixeleffect.h
RESOURCES = teaservice.qrc

package {
    maemo {
        applnk.files = teaservice.desktop
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
