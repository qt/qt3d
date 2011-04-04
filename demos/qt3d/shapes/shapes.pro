TEMPLATE = app
TARGET = shapes
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = shapes.cpp

package {
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
