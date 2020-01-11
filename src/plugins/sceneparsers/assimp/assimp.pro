TARGET = assimpsceneimport
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras 3danimation
DEFINES += BUILD_QT3D_MODULE

include(../../../3rdparty/assimp/assimp_dependency.pri)

HEADERS += \
    assimphelpers.h \
    assimpimporter.h

SOURCES += \
    assimphelpers.cpp \
    main.cpp \
    assimpimporter.cpp

DISTFILES += \
    assimp.json

PLUGIN_TYPE = sceneparsers
PLUGIN_CLASS_NAME = AssimpSceneImportPlugin
load(qt_plugin)
