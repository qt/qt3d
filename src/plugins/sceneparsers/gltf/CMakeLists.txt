# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gltf.pro.

#####################################################################
## GLTFSceneImportPlugin Plugin:
#####################################################################

qt_internal_add_plugin(GLTFSceneImportPlugin
    OUTPUT_NAME gltfsceneimport
    PLUGIN_TYPE sceneparsers
    SOURCES
        gltfimporter.cpp gltfimporter.h
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:gltf.pro:<TRUE>:
# DISTFILES = "gltf.json"
