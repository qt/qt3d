TARGET = gltfsceneparser

QT += core-private 3dcore 3dcore-private 3drender 3drender-private

HEADERS += \
    gltfparser.h

SOURCES += \
    gltfparser.cpp \
    main.cpp

DISTFILES += \
    gltf.json

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = GLTFSceneParserPlugin
load(qt_plugin)
