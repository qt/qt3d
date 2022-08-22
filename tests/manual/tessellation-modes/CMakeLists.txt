# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from tessellation-modes.pro.

#####################################################################
## tessellation-modes Binary:
#####################################################################

qt_internal_add_manual_test(tessellation-modes
    GUI
    SOURCES
        main.cpp
        tessellatedquadmesh.cpp tessellatedquadmesh.h
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(tessellation-modes_resource_files
    "BasicCamera.qml"
    "TessellatedQuad.qml"
    "TessellatedWireframeEffect.qml"
    "TessellatedWireframeMaterial.qml"
    "main.qml"
    "shaders/flat.frag"
    "shaders/isolines.tcs"
    "shaders/isolines.tes"
    "shaders/passthru.vert"
    "shaders/phongwireframe.frag"
    "shaders/quads.tcs"
    "shaders/quads.tes"
    "shaders/robustwireframe.geom"
    "shaders/triangles.tcs"
    "shaders/triangles.tes"
)

qt_internal_add_resource(tessellation-modes "tessellation-modes"
    PREFIX
        "/"
    FILES
        ${tessellation-modes_resource_files}
)


#### Keys ignored in scope 1:.:.:tessellation-modes.pro:<TRUE>:
# OTHER_FILES = "main.qml" "BasicCamera.qml" "TessellatedWireframeEffect.qml" "TessellatedWireframeMaterial.qml" "TessellatedQuad.qml" "shaders/*"

## Scopes:
#####################################################################
