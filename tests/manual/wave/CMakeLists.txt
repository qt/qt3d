# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(wave LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(wave
    main.cpp
)

set_target_properties(wave PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(wave PUBLIC
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
set(wave_resource_files
    "Background.qml"
    "BackgroundEffect.qml"
    "BasicCamera.qml"
    "Wave.qml"
    "WaveEffect.qml"
    "WaveForwardRenderer.qml"
    "WaveMaterial.qml"
    "main.qml"
    "shaders/background.frag"
    "shaders/background.vert"
    "shaders/ribbon.frag"
    "shaders/ribbon.vert"
    "shaders/ribbonwireframe.frag"
    "shaders/robustwireframe.geom"
)

qt6_add_resources(wave "wave"
    PREFIX
        "/"
    FILES
        ${wave_resource_files}
)

target_sources(wave PRIVATE ${wave_resource_files})
