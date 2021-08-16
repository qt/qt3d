# Generated from phong-cubes.pro.

cmake_minimum_required(VERSION 3.16)
project(phong-cubes LANGUAGES CXX)

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

qt_add_executable(phong-cubes
    main.cpp
)
set_target_properties(phong-cubes PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(phong-cubes PUBLIC
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
set(phong-cubes_resource_files
    "CubeEntity.qml"
    "main.qml"
)

qt6_add_resources(phong-cubes "phong-cubes"
    PREFIX
        "/"
    FILES
        ${phong-cubes_resource_files}
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

qt6_add_resources(phong-cubes "textures"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${textures_resource_files}
)

install(TARGETS phong-cubes
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
