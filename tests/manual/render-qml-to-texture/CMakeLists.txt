# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from render-qml-to-texture.pro.

#####################################################################
## render-qml-to-texture Binary:
#####################################################################

qt_internal_add_manual_test(render-qml-to-texture
    GUI
    SOURCES
        main.cpp
        planematerial.cpp planematerial.h
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickRender
        Qt::3DQuickScene2D
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(render-qml-to-texture_resource_files
    "OffscreenGui.qml"
    "TextRectangle.qml"
    "shaders/es2/texturing.frag"
    "shaders/es2/texturing.vert"
    "shaders/gl3/texturing.frag"
    "shaders/gl3/texturing.vert"
)

qt_internal_add_resource(render-qml-to-texture "render-qml-to-texture"
    PREFIX
        "/"
    FILES
        ${render-qml-to-texture_resource_files}
)


#### Keys ignored in scope 1:.:.:render-qml-to-texture.pro:<TRUE>:
# DISTFILES = "OffscreenGui.qml" "TextRectangle.qml"
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
