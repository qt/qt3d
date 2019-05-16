TARGET = gltfsceneimport
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras

HEADERS += \
    gltfimporter.h

SOURCES += \
    main.cpp \
    gltfimporter.cpp

DISTFILES += \
    gltf.json

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = GLTFSceneImportPlugin
load(qt_plugin)
