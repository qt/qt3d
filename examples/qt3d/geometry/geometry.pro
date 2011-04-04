TEMPLATE = app
TARGET = geometry
CONFIG += qt warn_on

!package: CONFIG += qt3d

SOURCES = geometryview.cpp \
    main.cpp \
    geometry.cpp \
    quadplane.cpp
HEADERS = geometryview.h \
    geometry.h \
    quadplane.h
RESOURCES = geometry.qrc

package {
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
