TARGET = assimpsceneparser
QT += core-private 3dcore 3dcore-private 3drender 3drender-private

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = AssimpParserPlugin
load(qt_plugin)

config_assimp {
    LIBS += -lassimp

    unix {
        CONFIG += link_pkgconfig
        PKGCONFIG_PRIVATE += assimp
    }
} else {
    include(../../../3rdparty/assimp/assimp.pri)
}

HEADERS += \
    assimphelpers.h \
    assimpparser.h

SOURCES += \
    assimpparser.cpp \
    assimphelpers.cpp \
    main.cpp

DISTFILES += \
    assimp.json
