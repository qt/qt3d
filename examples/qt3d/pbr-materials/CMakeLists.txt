# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(pbr-materials LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

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

qt6_add_resources(big_resources ../exampleresources/textures.qrc)
target_sources(pbr-materials PRIVATE ${big_resources})

install(TARGETS pbr-materials
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
