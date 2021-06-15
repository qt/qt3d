# Generated from rigged-simple.pro.

#####################################################################
## rigged-simple Binary:
#####################################################################

qt_internal_add_manual_test(rigged-simple
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
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
    "../../../examples/qt3d/exampleresources/assets/cubemaps/default/default_irradiance.dds"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/default/default_specular.dds"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negx.webp"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negy.webp"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negz.webp"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posx.webp"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posy.webp"
    "../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posz.webp"
)

qt_internal_add_resource(rigged-simple "cubemaps"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${cubemaps_resource_files}
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
)
set(envmaps_resource_files
    "../../../examples/qt3d/exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_irradiance.dds"
    "../../../examples/qt3d/exampleresources/assets/envmaps/cedar-bridge/cedar_bridge_specular.dds"
)

qt_internal_add_resource(rigged-simple "envmaps"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${envmaps_resource_files}
)
set(gltf_resource_files
    "../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
    "../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure0.bin"
    "../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
    "../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple0.bin"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/Wood_Cherry_Original_.jpg"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/_2004_old_vine_zinfandel_btl_xlg.jpg"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/artezin_bottle.jpg"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.bin"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.dae"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.gltf"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine0FS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine0VS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine1FS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine1VS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine2FS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine2VS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine3FS.glsl"
    "../../../examples/qt3d/exampleresources/assets/gltf/wine/wine3VS.glsl"
)

qt_internal_add_resource(rigged-simple "gltf"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${gltf_resource_files}
)


#### Keys ignored in scope 1:.:.:rigged-simple.pro:<TRUE>:
# DISTFILES = "skinnedPbr.vert"
# OTHER_FILES = "main.qml" "DefaultSceneEntity.qml" "SkinnedEntity.qml" "SkinnedPbrEffect.qml"

## Scopes:
#####################################################################
