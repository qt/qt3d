option(host_build)
!cross_compile:load(qt_build_paths)

SOURCES = qgltf.cpp

include(../../src/3rdparty/assimp/assimp_dependency.pri)

load(qt_tool)
