TEMPLATE = app
TARGET = nesting
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cubeview.cpp main.cpp
HEADERS = cubeview.h
RESOURCES = nesting.qrc

package {
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
