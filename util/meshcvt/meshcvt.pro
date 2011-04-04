TARGET = meshcvt

package {
    LIBS += -L../../src/threed -lQt3D
    INCLUDEPATH += ../../include
} else {
    CONFIG += qt3d
}

SOURCES += \
    meshcvt.cpp \

win32:DEFINES+=_CRT_SECURE_NO_WARNINGS
