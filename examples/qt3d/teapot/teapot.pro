TEMPLATE = app
TARGET = teapot
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = teapotview.cpp main.cpp
HEADERS = teapotview.h

package {
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
