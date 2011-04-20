TEMPLATE = lib
TARGET  = qthreedqmlplugin
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
    target.path += $$[QT_INSTALL_IMPORTS]/Qt3D
    INSTALLS += target
    QT += declarative opengl
} else {
    CONFIG += qt3dquick qt3d
    DESTDIR = $$[QT_INSTALL_IMPORTS]/Qt3D
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
    threed.cpp \
    qt3dnamespace.cpp \
    scale3d.cpp \
    viewport.cpp \
    qgraphicslookattransform.cpp \
    shaderprogram.cpp

HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    qgraphicslookattransform.h \
    shaderprogram.h \
    shaderprogram_p.h

# See the file README.library_xml for more on this
qdeclarativesources.files += \
    qmldir \
    library.xml

!package {
    qdeclarative_in_place.input = qdeclarativesources.files
    qdeclarative_in_place.output = $$[QT_INSTALL_IMPORTS]/Qt3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    qdeclarative_in_place.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    qdeclarative_in_place.CONFIG += no_link_no_clean
    qdeclarative_in_place.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += qdeclarative_in_place
} else {
    qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D
    INSTALLS += qdeclarativesources
}
