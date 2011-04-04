TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin qt3dquick

package {
    LIBS += -L../../quick3d -L../../threed
    INCLUDEPATH += ../../../include
} else {
    CONFIG += qt3dquick
}

win32 {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}

SOURCES += \
    sphere.cpp \
    shapes.cpp \
    cylinder.cpp \
    capsule.cpp \
    line.cpp \
    point.cpp
HEADERS += \
    sphere.h \
    cylinder.h \
    capsule.h \
    line.h \
    point.h

qdeclarativesources.files += \
    qmldir \
    library.xml \
    Cube.qml \
    cube.obj \
    Quad.qml \
    quad.obj \
    Teapot.qml \
    teapot.bez
# see the file README.library_xml for more on library.xml

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

INSTALLS += qdeclarativesources target
