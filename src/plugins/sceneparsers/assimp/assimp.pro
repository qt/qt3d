TARGET = assimpsceneparser

QT += core-private 3dcore 3dcore-private 3drender 3drender-private

include(../../../3rdparty/assimp/assimp_dependency.pri)

HEADERS += \
    assimphelpers.h \
    assimpparser.h

SOURCES += \
    assimpparser.cpp \
    assimphelpers.cpp \
    main.cpp

DISTFILES += \
    assimp.json

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = AssimpParserPlugin
load(qt_plugin)
