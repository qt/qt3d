TARGET = gltfsceneparser
QT += core-private 3dcore 3dcore-private 3drenderer 3drenderer-private

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = GLTFParser
load(qt_plugin)

HEADERS += \
    gltfparser_p.h

SOURCES += \
    gltfparser.cpp
