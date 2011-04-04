TEMPLATE = app
TARGET = cubehouse
CONFIG += qt warn_on
!package: CONFIG += qt3d

SOURCES = cubeview.cpp main.cpp projectivetextureeffect.cpp
HEADERS = cubeview.h projectivetextureeffect.h
win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
RESOURCES = cube.qrc

OTHER_FILES += \
    shaders/objectlineartexgen.frag \
    shaders/objectlineartexgen.vert

package {
    LIBS += -L../../../src/threed -lQt3D
    INCLUDEPATH += ../../../include
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}
