TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin

# See the README in the root dir re this code
package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../../threed
        LIBS += -framework Qt3DQuick -F../../quick3d
        INCLUDEPATH += ../../threed/Qt3D.framework/Versions/1/Headers
        INCLUDEPATH += ../../quick3d/Qt3DQuick.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                LIBS += ..\\..\\threed\\debug\\Qt3Dd.lib
                LIBS += ..\\..\\quick3d\\debug\\Qt3DQuickd.lib
            } else {
                LIBS += ..\\..\\threed\\release\\Qt3D.lib
                LIBS += ..\\..\\quick3d\\release\\Qt3DQuick.lib
            }
        } else {
            LIBS += -L../../threed -lQt3D
            LIBS += -L../../quick3d -lQt3DQuick
        }
        INCLUDEPATH += ../../../include/Qt3D
        INCLUDEPATH += ../../../include/Qt3DQuick
    }
    target.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes
    INSTALLS += target
    QT += declarative
} else {
    CONFIG += qt3dquick qt3d
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
    LIBS += -lQt3D -lQt3DQuick
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x20031E9C
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
    plugins.qmltypes \
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
} else {
    qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes
    INSTALLS += qdeclarativesources
}

OTHER_FILES += \
    README.plugins_types \
    README.library_xml
