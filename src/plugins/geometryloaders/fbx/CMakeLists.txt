# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from fbx.pro.

#####################################################################
## fbxGeometryLoaderPlugin Plugin:
#####################################################################

qt_internal_add_plugin(fbxGeometryLoaderPlugin
    OUTPUT_NAME fbxgeometryloader
    PLUGIN_TYPE geometryloaders
    SOURCES
        fbxgeometryloader.cpp fbxgeometryloader.h
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        fbx
)

#### Keys ignored in scope 1:.:.:fbx.pro:<TRUE>:
# DISTFILES = "fbx.json"
