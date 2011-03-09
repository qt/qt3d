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
    floatingitem.cpp \
    floatingitemeffect.cpp \
    stereoview.cpp \
    stereovieweffect.cpp \
    qgraphicslookattransform.cpp \
    shaderprogram.cpp

HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    floatingitem.h \
    floatingitemeffect.h \
    stereoview.h \
    stereovieweffect.h \
    qgraphicslookattransform.h \
    shaderprogram.h \
    shaderprogram_p.h

scenegraph {
    SOURCES += \
        floatingitem_sg.cpp \
        floatingitemnode_sg.cpp \
        stereoinfo_sg.cpp \
        viewport_sg.cpp \
        viewportfbonode_sg.cpp
    HEADERS += \
        floatingitem_sg.h \
        floatingitemnode_sg.h \
        stereoinfo_sg.h \
        viewport_sg.h \
        viewportfbonode_sg.h
}

qdeclarativesources.files += \
    qmldir

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D

target.path += $$[QT_INSTALL_IMPORTS]/Qt3D

INSTALLS += qdeclarativesources target
