TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin qt3dquick

package {
    LIBS += -L../../quick3d -L../../threed -lQt3D -lQt3DQuick
    INCLUDEPATH += ../../../include
} else {
    CONFIG += qt3dquick
    DESTDIR = $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes
}

win32 {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    CONFIG += epocallowdlldata
    MMP_RULES += EXPORTUNFROZEN
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

!package {
    qdeclarative_in_place.input = qdeclarativesources.files
    qdeclarative_in_place.output = $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    qdeclarative_in_place.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    qdeclarative_in_place.CONFIG += no_link_no_clean
    qdeclarative_in_place.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += qdeclarative_in_place
}

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes

INSTALLS += qdeclarativesources target
