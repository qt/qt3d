# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gltf.pro.

#####################################################################
## gltf Binary:
#####################################################################

qt_internal_add_manual_test(gltf
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
set(gltf_example_resource_files
    "Wine.qml"
    "main.qml"
)

qt_internal_add_resource(gltf "gltf_example"
    PREFIX
        "/"
    FILES
        ${gltf_example_resource_files}
)

set(gltf_resource_files
    "../exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
    "../exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure0.bin"
    "../exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
    "../exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple0.bin"
    "../exampleresources/assets/gltf/wine/Wood_Cherry_Original_.jpg"
    "../exampleresources/assets/gltf/wine/_2004_old_vine_zinfandel_btl_xlg.jpg"
    "../exampleresources/assets/gltf/wine/artezin_bottle.jpg"
    "../exampleresources/assets/gltf/wine/wine.bin"
    "../exampleresources/assets/gltf/wine/wine.dae"
    "../exampleresources/assets/gltf/wine/wine.gltf"
    "../exampleresources/assets/gltf/wine/wine0FS.glsl"
    "../exampleresources/assets/gltf/wine/wine0VS.glsl"
    "../exampleresources/assets/gltf/wine/wine1FS.glsl"
    "../exampleresources/assets/gltf/wine/wine1VS.glsl"
    "../exampleresources/assets/gltf/wine/wine2FS.glsl"
    "../exampleresources/assets/gltf/wine/wine2VS.glsl"
    "../exampleresources/assets/gltf/wine/wine3FS.glsl"
    "../exampleresources/assets/gltf/wine/wine3VS.glsl"
)

qt_internal_add_resource(gltf "gltf"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${gltf_resource_files}
)


#### Keys ignored in scope 1:.:.:gltf.pro:<TRUE>:
# OTHER_FILES = "main.qml" "Wine.qml"

## Scopes:
#####################################################################
