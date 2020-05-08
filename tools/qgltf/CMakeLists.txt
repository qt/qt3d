# Generated from qgltf.pro.

#####################################################################
## qgltf Tool:
#####################################################################

qt_add_tool(qgltf
    SOURCES
        qgltf.cpp
    EXCEPTIONS # special case
    PUBLIC_LIBRARIES
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qgltf.pro:<TRUE>:
# _OPTION = "host_build"

## Scopes:
#####################################################################
include(../../src/3rdparty/assimp/assimp.cmake)
qt3d_extend_target_for_assimp(qgltf)
