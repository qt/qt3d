# Generated from pbr-materials.pro.

cmake_minimum_required(VERSION 3.16)
project(pbr-materials LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DCore)
find_package(Qt6 COMPONENTS 3DRender)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(pbr-materials
    main.cpp
)
set_target_properties(pbr-materials PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(pbr-materials PUBLIC
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
set(materials_resource_files
    "BasicCamera.qml"
    "Lights.qml"
    "TrefoilKnot.qml"
    "main.qml"
)

qt6_add_resources(pbr-materials "materials"
    PREFIX
        "/"
    FILES
        ${materials_resource_files}
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

qt6_add_resources(pbr-materials "cubemaps"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${cubemaps_resource_files}
)
set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt6_add_resources(pbr-materials "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)
set(textures_resource_files
    "../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_basecolor.png"
    "../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_metallic.png"
    "../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_normal.png"
    "../exampleresources/assets/textures/aluminium_random_brushed/aluminium_random_brushed_roughness.png"
    "../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_basecolor.png"
    "../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_metallic.png"
    "../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_normal.png"
    "../exampleresources/assets/textures/american_walnut_crown_cut/american_walnut_crown_cut_roughness.png"
    "../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_ambient_occlusion.png"
    "../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_basecolor.png"
    "../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_metallic.png"
    "../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_normal.png"
    "../exampleresources/assets/textures/ceramic_small_diamond/ceramic_small_diamond_roughness.png"
    "../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_basecolor.png"
    "../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_metallic.png"
    "../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_normal.png"
    "../exampleresources/assets/textures/ceramic_tiles_brown_tomato/ceramic_tiles_brown_tomato_roughness.png"
    "../exampleresources/assets/textures/copper_brushed/copper_brushed_basecolor.png"
    "../exampleresources/assets/textures/copper_brushed/copper_brushed_metallic.png"
    "../exampleresources/assets/textures/copper_brushed/copper_brushed_normal.png"
    "../exampleresources/assets/textures/copper_brushed/copper_brushed_roughness.png"
    "../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_basecolor.png"
    "../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_metallic.png"
    "../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_normal.png"
    "../exampleresources/assets/textures/gold_leaf_waste/gold_leaf_waste_roughness.png"
    "../exampleresources/assets/textures/no-ao.png"
)

qt6_add_resources(pbr-materials "textures"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${textures_resource_files}
)

install(TARGETS pbr-materials
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
