# Generated from skybox.pro.

#####################################################################
## skybox Binary:
#####################################################################

qt_internal_add_manual_test(skybox
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
set(skybox_resource_files
    "Skybox.qml"
    "main.qml"
)

qt_internal_add_resource(skybox "skybox"
    PREFIX
        "/"
    FILES
        ${skybox_resource_files}
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

qt_internal_add_resource(skybox "cubemaps"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${cubemaps_resource_files}
)


## Scopes:
#####################################################################
