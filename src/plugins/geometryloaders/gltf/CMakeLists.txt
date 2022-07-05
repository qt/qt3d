# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from gltf.pro.

#####################################################################
## GLTFGeometryLoaderPlugin Plugin:
#####################################################################

qt_internal_add_plugin(GLTFGeometryLoaderPlugin
    OUTPUT_NAME gltfgeometryloader
    PLUGIN_TYPE geometryloaders
    SOURCES
        gltfgeometryloader.cpp gltfgeometryloader.h
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:gltf.pro:<TRUE>:
# DISTFILES = "gltf.json"
