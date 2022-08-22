# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from scene2d.pro.

#####################################################################
## Scene2DPlugin Plugin:
#####################################################################

qt_internal_add_plugin(Scene2DPlugin
    OUTPUT_NAME scene2d
    PLUGIN_TYPE renderplugins
    SOURCES
        main.cpp
        scene2dplugin.cpp scene2dplugin.h
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuickScene2D
        Qt::3DQuickScene2DPrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:scene2d.pro:<TRUE>:
# DISTFILES = "scene2dplugin.json"
