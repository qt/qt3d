# Generated from transparency-qml-scene3d.pro.

#####################################################################
## transparency-qml-scene3d Binary:
#####################################################################

qt_internal_add_manual_test(transparency-qml-scene3d
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

qt_internal_add_resource(transparency-qml-scene3d "transparency-qml-scene3d"
    PREFIX
        "/"
    FILES
        ${transparency-qml-scene3d_resource_files}
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

qt_internal_add_resource(transparency-qml-scene3d "cubemaps"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${cubemaps_resource_files}
)


## Scopes:
#####################################################################
