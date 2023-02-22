# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rigged-simple.pro.

#####################################################################
## rigged-simple Binary:
#####################################################################

qt_internal_add_manual_test(rigged-simple
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DAnimation
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
set(rigged-simple_resource_files
    "DefaultSceneEntity.qml"
    "SkinnedEntity.qml"
    "SkinnedPbrEffect.qml"
    "main.qml"
    "skinnedPbr.vert"
)

qt_internal_add_resource(rigged-simple "rigged-simple"
    PREFIX
        "/"
    FILES
        ${rigged-simple_resource_files}
)
set(cubemaps_resource_files
    "../exampleresources/assets/cubemaps/default/default_irradiance.dds"
    "../exampleresources/assets/cubemaps/default/default_specular.dds"
    "../exampleresources/assets/cubemaps/miramar/miramar_negx.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_negy.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_negz.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posx.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posy.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posz.webp"
)

qt_internal_add_resource(rigged-simple "cubemaps"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${cubemaps_resource_files}
)
set_source_files_properties("../exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
)
set_source_files_properties("../exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
)
set(envmaps_resource_files
    "../exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
    "../exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
)

qt_internal_add_resource(rigged-simple "envmaps"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${envmaps_resource_files}
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

qt_internal_add_resource(rigged-simple "gltf"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${gltf_resource_files}
)


#### Keys ignored in scope 1:.:.:rigged-simple.pro:<TRUE>:
# DISTFILES = "skinnedPbr.vert"
# OTHER_FILES = "main.qml" "DefaultSceneEntity.qml" "SkinnedEntity.qml" "SkinnedPbrEffect.qml"

## Scopes:
#####################################################################
