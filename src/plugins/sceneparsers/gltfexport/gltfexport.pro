TARGET = gltfsceneexport
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras
DEFINES += BUILD_QT3D_MODULE

HEADERS += \
    gltfexporter.h

SOURCES += \
    main.cpp \
    gltfexporter.cpp

DISTFILES += \
    gltfexport.json

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = GLTFSceneExportPlugin
load(qt_plugin)
