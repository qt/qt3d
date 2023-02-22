# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(compute-particles LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DRender Core Gui Qml Quick)

qt_add_executable(compute-particles
    main.cpp
)

set_target_properties(compute-particles PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(compute-particles PUBLIC
    Qt::3DCore
    Qt::3DInput
    Qt::3DQuick
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(compute-particles_resource_files
    "ComputeFrameGraph.qml"
    "ComputeMaterial.qml"
    "ParticlesScene.qml"
    "main.qml"
    "shaders/gl43/particles.comp"
    "shaders/gl43/particles.frag"
    "shaders/gl43/particles.vert"
    "shaders/gl45/particles.comp"
    "shaders/gl45/particles.frag"
    "shaders/gl45/particles.vert"
)

qt6_add_resources(compute-particles "compute-particles"
    PREFIX
        "/"
    FILES
        ${compute-particles_resource_files}
)
