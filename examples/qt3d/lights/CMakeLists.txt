# Generated from lights.pro.

cmake_minimum_required(VERSION 3.14)
project(lights LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS 3DCore)
find_package(Qt6 COMPONENTS 3DRender)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(lights
    main.cpp
)
set_target_properties(lights PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(lights PUBLIC
    Qt::3DCore
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
set(lights_resource_files
    "PlaneEntity.qml"
    "SimpleForwardRenderer.qml"
    "main.qml"
)

qt6_add_resources(lights "lights"
    PREFIX
        "/"
    FILES
        ${lights_resource_files}
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

qt6_add_resources(lights "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)
set_source_files_properties("../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/aluminium_random_brushed/aluminium_random_brushed_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/aluminium_random_brushed/aluminium_random_brushed_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_ambient_occlusion.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_small_diamond/ceramic_small_diamond_ambient_occlusion.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_small_diamond/ceramic_small_diamond_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_small_diamond/ceramic_small_diamond_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/copper_brushed/copper_brushed_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/copper_brushed/copper_brushed_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/copper_brushed/copper_brushed_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/copper_brushed/copper_brushed_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/copper_brushed/copper_brushed_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/copper_brushed/copper_brushed_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/copper_brushed/copper_brushed_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/copper_brushed/copper_brushed_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_basecolor.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/gold_leaf_waste/gold_leaf_waste_basecolor.png"
)
set_source_files_properties("../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_metallic.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/gold_leaf_waste/gold_leaf_waste_metallic.png"
)
set_source_files_properties("../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_normal.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/gold_leaf_waste/gold_leaf_waste_normal.png"
)
set_source_files_properties("../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_roughness.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/gold_leaf_waste/gold_leaf_waste_roughness.png"
)
set_source_files_properties("../exampleresources/assets/textures/no-ao.png"
    PROPERTIES QT_RESOURCE_ALIAS "assets/textures/no-ao.png"
)
set(textures_resource_files
    "assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png"
    "assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png"
    "assets/textures/aluminium_random_brushed/aluminium_random_brushed_normal.png"
    "assets/textures/aluminium_random_brushed/aluminium_random_brushed_roughness.png"
    "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_basecolor.png"
    "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_metallic.png"
    "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_normal.png"
    "assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_roughness.png"
    "assets/textures/ceramic_small_diamond/ceramic_small_diamond_ambient_occlusion.png"
    "assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
    "assets/textures/ceramic_small_diamond/ceramic_small_diamond_metallic.png"
    "assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"
    "assets/textures/ceramic_small_diamond/ceramic_small_diamond_roughness.png"
    "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_basecolor.png"
    "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_metallic.png"
    "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_normal.png"
    "assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_roughness.png"
    "assets/textures/copper_brushed/copper_brushed_basecolor.png"
    "assets/textures/copper_brushed/copper_brushed_metallic.png"
    "assets/textures/copper_brushed/copper_brushed_normal.png"
    "assets/textures/copper_brushed/copper_brushed_roughness.png"
    "assets/textures/gold_leaf_waste/gold_leaf_waste_basecolor.png"
    "assets/textures/gold_leaf_waste/gold_leaf_waste_metallic.png"
    "assets/textures/gold_leaf_waste/gold_leaf_waste_normal.png"
    "assets/textures/gold_leaf_waste/gold_leaf_waste_roughness.png"
    "assets/textures/no-ao.png"
)

qt6_add_resources(lights "textures"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${textures_resource_files}
)

install(TARGETS lights
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
