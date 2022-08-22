# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gltfexport.pro.

#####################################################################
## GLTFSceneExportPlugin Plugin:
#####################################################################

qt_internal_add_plugin(GLTFSceneExportPlugin
    OUTPUT_NAME gltfsceneexport
    PLUGIN_TYPE sceneparsers
    SOURCES
        gltfexporter.cpp gltfexporter.h
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

#### Keys ignored in scope 1:.:.:gltfexport.pro:<TRUE>:
# DISTFILES = "gltfexport.json"
