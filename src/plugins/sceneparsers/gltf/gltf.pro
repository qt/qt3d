TARGET = gltfsceneparser
QT += core-private 3dcore 3dcore-private 3drender 3drender-private

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = GLTFParser
load(qt_plugin)

HEADERS += \
    gltfparser_p.h

SOURCES += \
    gltfparser.cpp
