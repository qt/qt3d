# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from transforms-qml.pro.

#####################################################################
## transforms-qml Binary:
#####################################################################

qt_internal_add_manual_test(transforms-qml
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
set(transforms-qml_resource_files
    "RenderableEntity.qml"
    "TemporaryCamera.qml"
    "TrefoilKnot.qml"
    "main.qml"
)

qt_internal_add_resource(transforms-qml "transforms-qml"
    PREFIX
        "/"
    FILES
        ${transforms-qml_resource_files}
)
set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt_internal_add_resource(transforms-qml "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)


#### Keys ignored in scope 1:.:.:transforms-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "RenderableEntity.qml" "TemporaryCamera.qml" "TrefoilKnot.qml"

## Scopes:
#####################################################################
