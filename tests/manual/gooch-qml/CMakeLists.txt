# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gooch-qml.pro.

#####################################################################
## gooch-qml Binary:
#####################################################################

qt_internal_add_manual_test(gooch-qml
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
set(gooch-qml_resource_files
    "MyEntity.qml"
    "main.qml"
)

qt_internal_add_resource(gooch-qml "gooch-qml"
    PREFIX
        "/"
    FILES
        ${gooch-qml_resource_files}
)

set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt_internal_add_resource(gooch-qml "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)


#### Keys ignored in scope 1:.:.:gooch-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "MyEntity.qml"

## Scopes:
#####################################################################
