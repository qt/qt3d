option(host_build)
!cross_compile:load(qt_build_paths)

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES *= QT_NO_FOREACH

SOURCES = qgltf.cpp

include(../../src/3rdparty/assimp/assimp_dependency.pri)

load(qt_tool)
