# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(advancedcustommaterial LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 3DRender Core Gui Qml Quick)

qt_add_executable(advancedcustommaterial
    main.cpp
)

set_target_properties(advancedcustommaterial PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(advancedcustommaterial PUBLIC
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "SceneRoot.qml"
    "Water.qml"
    "WaterMaterial.qml"
    "main.qml"
)

qt6_add_resources(advancedcustommaterial "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

target_sources(advancedcustommaterial PRIVATE ${qml_resource_files}) # just to show qml files in creator

set(models_resource_files
    "models/waterPlane.obj"
)

qt6_add_resources(advancedcustommaterial "models"
    PREFIX
        "/"
    FILES
        ${models_resource_files}
)

set(textures_resource_files
    "textures/WaterDiffuse.jpg"
    "textures/WaterNormal.jpg"
    "textures/WaterSpecular.jpg"
    "textures/Waterwave.jpg"
    "textures/foam.jpg"
    "textures/sky.jpg"
)

qt6_add_resources(advancedcustommaterial "textures"
    PREFIX
        "/"
    FILES
        ${textures_resource_files}
)

set(shaders_resource_files
    "shaders/es2/water.frag"
    "shaders/es2/water.vert"
    "shaders/gl3/water.frag"
    "shaders/gl3/water.vert"
    "shaders/rhi/water.frag"
    "shaders/rhi/water.vert"
)

qt6_add_resources(advancedcustommaterial "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)
