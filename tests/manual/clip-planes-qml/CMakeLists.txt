# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from clip-planes-qml.pro.

#####################################################################
## clip-planes-qml Binary:
#####################################################################

qt_internal_add_manual_test(clip-planes-qml
    GUI
    SOURCES
        main.cpp
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
set(clip-planes-qml_resource_files
    "CappingMaterialEffect.qml"
    "ClipCappingFrameGraph.qml"
    "ClipMaterialEffect.qml"
    "ClipPlaneEntity.qml"
    "ClippingPlanes.qml"
    "PlaneVisualizationMaterial.qml"
    "capping.frag"
    "capping.geom"
    "capping.vert"
    "main.qml"
    "passthrough.frag"
    "phong-clip.frag"
    "phong-clip.vert"
    "default.vert"
    "phongalpha.frag"
    "light.inc.frag"
)

qt_internal_add_resource(clip-planes-qml "clip-planes-qml"
    PREFIX
        "/"
    FILES
        ${clip-planes-qml_resource_files}
)
set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt_internal_add_resource(clip-planes-qml "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)


## Scopes:
#####################################################################
