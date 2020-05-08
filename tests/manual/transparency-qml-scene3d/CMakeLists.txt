# Generated from transparency-qml-scene3d.pro.

#####################################################################
## transparency-qml-scene3d Binary:
#####################################################################

qt_add_manual_test(transparency-qml-scene3d
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
set(transparency-qml-scene3d_resource_files
    "Scene.qml"
    "main.qml"
)

qt_add_resource(transparency-qml-scene3d "transparency-qml-scene3d"
    PREFIX
        "/"
    FILES
        ${transparency-qml-scene3d_resource_files}
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/default/default_irradiance.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/default/default_irradiance.dds"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/default/default_specular.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/default/default_specular.dds"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negx.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negx.webp"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negy.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negy.webp"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_negz.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negz.webp"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posx.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_posx.webp"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posy.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_posy.webp"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/cubemaps/miramar/miramar_posz.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_posz.webp"
)
set(cubemaps_resource_files
    "assets/cubemaps/default/default_irradiance.dds"
    "assets/cubemaps/default/default_specular.dds"
    "assets/cubemaps/miramar/miramar_negx.webp"
    "assets/cubemaps/miramar/miramar_negy.webp"
    "assets/cubemaps/miramar/miramar_negz.webp"
    "assets/cubemaps/miramar/miramar_posx.webp"
    "assets/cubemaps/miramar/miramar_posy.webp"
    "assets/cubemaps/miramar/miramar_posz.webp"
)

qt_add_resource(transparency-qml-scene3d "cubemaps"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${cubemaps_resource_files}
)


## Scopes:
#####################################################################
