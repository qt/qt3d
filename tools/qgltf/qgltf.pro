option(host_build)
load(qt_tool)

SOURCES = $$PWD/qgltf.cpp

include(../../src/3rdparty/assimp/assimp.pri)

mkspecs_features.files    = $$PWD/qgltf.prf
mkspecs_features.path     = $$[QT_HOST_DATA]/mkspecs/features
INSTALLS                  += mkspecs_features
