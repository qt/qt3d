# Generated from gltf.pro.

#####################################################################
## gltf Binary:
#####################################################################

qt_add_manual_test(gltf
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
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

qt_add_resource(gltf "gltf_example"
    PREFIX
        "/"
    FILES
        ${gltf_example_resource_files}
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedFigure/RiggedFigure0.bin"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/2.0/RiggedFigure/RiggedFigure0.bin"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/2.0/RiggedSimple/RiggedSimple0.bin"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/2.0/RiggedSimple/RiggedSimple0.bin"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/Wood_Cherry_Original_.jpg"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/Wood_Cherry_Original_.jpg"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/_2004_old_vine_zinfandel_btl_xlg.jpg"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/_2004_old_vine_zinfandel_btl_xlg.jpg"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/artezin_bottle.jpg"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/artezin_bottle.jpg"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.bin"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine.bin"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.dae"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine.dae"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine.gltf"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine.gltf"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine0FS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine0FS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine0VS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine0VS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine1FS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine1FS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine1VS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine1VS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine2FS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine2FS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine2VS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine2VS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine3FS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine3FS.glsl"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/gltf/wine/wine3VS.glsl"
    PROPERTIES QT_RESOURCE_ALIAS "assets/gltf/wine/wine3VS.glsl"
)
set(gltf_resource_files
    "assets/gltf/2.0/RiggedFigure/RiggedFigure.gltf"
    "assets/gltf/2.0/RiggedFigure/RiggedFigure0.bin"
    "assets/gltf/2.0/RiggedSimple/RiggedSimple.gltf"
    "assets/gltf/2.0/RiggedSimple/RiggedSimple0.bin"
    "assets/gltf/wine/Wood_Cherry_Original_.jpg"
    "assets/gltf/wine/_2004_old_vine_zinfandel_btl_xlg.jpg"
    "assets/gltf/wine/artezin_bottle.jpg"
    "assets/gltf/wine/wine.bin"
    "assets/gltf/wine/wine.dae"
    "assets/gltf/wine/wine.gltf"
    "assets/gltf/wine/wine0FS.glsl"
    "assets/gltf/wine/wine0VS.glsl"
    "assets/gltf/wine/wine1FS.glsl"
    "assets/gltf/wine/wine1VS.glsl"
    "assets/gltf/wine/wine2FS.glsl"
    "assets/gltf/wine/wine2VS.glsl"
    "assets/gltf/wine/wine3FS.glsl"
    "assets/gltf/wine/wine3VS.glsl"
)

qt_add_resource(gltf "gltf"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${gltf_resource_files}
)


#### Keys ignored in scope 1:.:.:gltf.pro:<TRUE>:
# OTHER_FILES = "main.qml" "Wine.qml"

## Scopes:
#####################################################################
