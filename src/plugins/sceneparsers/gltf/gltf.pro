TARGET = gltfsceneimport
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras
DEFINES += BUILD_QT3D_MODULE

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
