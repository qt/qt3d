# Generated from anaglyph-rendering.pro.

cmake_minimum_required(VERSION 3.14)
project(anaglyph-rendering LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(anaglyph-rendering
    main.cpp
)
target_link_libraries(anaglyph-rendering PUBLIC
    Qt::3DCore
    Qt::3DExtras
    Qt::3DInput
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(resources_resource_files
    "StereoCamera.qml"
    "StereoFrameGraph.qml"
    "main.qml"
)

qt6_add_resources(anaglyph-rendering "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)
set_source_files_properties("../exampleresources/assets/cubemaps/default/default_irradiance.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/default/default_irradiance.dds"
)
set_source_files_properties("../exampleresources/assets/cubemaps/default/default_specular.dds"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/default/default_specular.dds"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_negx.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negx.webp"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_negy.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negy.webp"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_negz.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_negz.webp"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_posx.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_posx.webp"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_posy.webp"
    PROPERTIES QT_RESOURCE_ALIAS "assets/cubemaps/miramar/miramar_posy.webp"
)
set_source_files_properties("../exampleresources/assets/cubemaps/miramar/miramar_posz.webp"
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

qt6_add_resources(anaglyph-rendering "cubemaps"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${cubemaps_resource_files}
)
set_source_files_properties("../exampleresources/assets/obj/ball.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/ball.obj"
)
set_source_files_properties("../exampleresources/assets/obj/material-sphere.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/material-sphere.obj"
)
set_source_files_properties("../exampleresources/assets/obj/plane-10x10.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/plane-10x10.obj"
)
set_source_files_properties("../exampleresources/assets/obj/toyplane.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/toyplane.obj"
)
set_source_files_properties("../exampleresources/assets/obj/trefoil.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/trefoil.obj"
)
set(obj_resource_files
    "assets/obj/ball.obj"
    "assets/obj/material-sphere.obj"
    "assets/obj/plane-10x10.obj"
    "assets/obj/toyplane.obj"
    "assets/obj/trefoil.obj"
)

qt6_add_resources(anaglyph-rendering "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)

install(TARGETS anaglyph-rendering
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
