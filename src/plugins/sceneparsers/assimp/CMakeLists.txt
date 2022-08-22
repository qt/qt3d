# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from assimp.pro.

#####################################################################
## AssimpSceneImportPlugin Plugin:
#####################################################################

qt_internal_add_plugin(AssimpSceneImportPlugin
    OUTPUT_NAME assimpsceneimport
    PLUGIN_TYPE sceneparsers
    EXCEPTIONS # special case
    SOURCES
        assimphelpers.cpp assimphelpers.h
        assimpimporter.cpp assimpimporter.h
        main.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:assimp.pro:<TRUE>:
# DISTFILES = "assimp.json"

## Scopes:
#####################################################################
include(../../../3rdparty/assimp/assimp.cmake)
qt3d_extend_target_for_assimp(AssimpSceneImportPlugin)
