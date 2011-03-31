TEMPLATE = lib
TARGET  = qthreedqmlplugin
CONFIG += qt plugin qt3d qt3dquick

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
QT += declarative opengl network
#VERSION = 1.0.0

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

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D

INSTALLS += qdeclarativesources target
