# Generated from anaglyph-rendering.pro.

cmake_minimum_required(VERSION 3.16)
project(anaglyph-rendering LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DExtras)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(anaglyph-rendering
    main.cpp
)
set_target_properties(anaglyph-rendering PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
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

qt6_add_resources(anaglyph-rendering "cubemaps"
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
