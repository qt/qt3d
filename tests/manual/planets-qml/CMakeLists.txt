# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(planets-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DQuickInput 3DQuickRender 3DRender Concurrent Core Gui Network Qml Quick)

qt_add_executable(planets-qml
    main.cpp
    networkcontroller.cpp networkcontroller.h
)

set_target_properties(planets-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(planets-qml PUBLIC
    Qt::3DCore
    Qt::3DInput
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::3DQuickInput
    Qt::3DQuickRender
    Qt::3DRender
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Qml
    Qt::Quick
)

# Resources:
set(planets-qml_resource_files
    "AppleTVInput.qml"
    "FpsDisplay.qml"
    "InfoSheet.qml"
    "Planet.qml"
    "PlanetButton.qml"
    "PlanetEffect.qml"
    "PlanetFrameGraph.qml"
    "PlanetMaterial.qml"
    "PlanetsLight.qml"
    "PlanetsMain.qml"
    "Ring.qml"
    "ShadowEffect.qml"
    "SolarSystem.qml"
    "StyledSlider.qml"
    "SunEffect.qml"
    "meshes/ring.obj"
    "meshes/starfield.obj"
    "planets.js"
    "shaders/es2/planetD.frag"
    "shaders/es2/planetD.vert"
    "shaders/es2/planetDB.frag"
    "shaders/es2/planetDB.vert"
    "shaders/es2/planetDS.frag"
    "shaders/es2/planetDSB.frag"
    "shaders/es2/sun.frag"
    "shaders/es2/sun.vert"
    "shaders/gl3/planetD.frag"
    "shaders/gl3/planetD.vert"
    "shaders/gl3/planetDB.frag"
    "shaders/gl3/planetDB.vert"
    "shaders/gl3/planetDS.frag"
    "shaders/gl3/planetDSB.frag"
    "shaders/gl3/planetDShadow.frag"
    "shaders/gl3/planetDShadow.vert"
    "shaders/gl3/shadowmap.frag"
    "shaders/gl3/shadowmap.vert"
    "shaders/gl3/sun.frag"
    "shaders/gl3/sun.vert"
    "shaders/rhi/planetD.frag"
    "shaders/rhi/planetD.vert"
    "shaders/rhi/planetDB.frag"
    "shaders/rhi/planetDB.vert"
    "shaders/rhi/planetDS.frag"
    "shaders/rhi/planetDSB.frag"
    "shaders/rhi/planetDShadow.frag"
    "shaders/rhi/planetDShadow.vert"
    "shaders/rhi/shadowmap.frag"
    "shaders/rhi/shadowmap.vert"
    "shaders/rhi/sun.frag"
    "shaders/rhi/sun.vert"
)

qt6_add_resources(planets-qml "planets-qml"
    PREFIX
        "/"
    FILES
        ${planets-qml_resource_files}
)

set(planets-qml-images_resource_files
    "images/earth.png"
    "images/jupiter.png"
    "images/mars.png"
    "images/mercury.png"
    "images/nasa/uranusringcolortrans.png"
    "images/neptune.png"
    "images/saturn.png"
    "images/solarsystemscope/earthcloudmapcolortrans.png"
    "images/solarsystemscope/earthcloudmapspec.jpg"
    "images/solarsystemscope/earthmap2k.jpg"
    "images/solarsystemscope/earthnormal2k.jpg"
    "images/solarsystemscope/earthspec2k.jpg"
    "images/solarsystemscope/galaxy_starfield.jpg"
    "images/solarsystemscope/jupitermap.jpg"
    "images/solarsystemscope/license.txt"
    "images/solarsystemscope/marsmap2k.jpg"
    "images/solarsystemscope/marsnormal2k.jpg"
    "images/solarsystemscope/mercurymap.jpg"
    "images/solarsystemscope/mercurynormal.jpg"
    "images/solarsystemscope/moonmap2k.jpg"
    "images/solarsystemscope/moonnormal2k.jpg"
    "images/solarsystemscope/neptunemap.jpg"
    "images/solarsystemscope/saturnmap.jpg"
    "images/solarsystemscope/saturnringcolortrans.png"
    "images/solarsystemscope/sunmap.jpg"
    "images/solarsystemscope/uranusmap.jpg"
    "images/solarsystemscope/venusmap.jpg"
    "images/solarsystemscope/venusnormal.jpg"
    "images/sun.png"
    "images/uranus.png"
    "images/venus.png"
)

qt6_add_resources(planets-qml "planets-qml-images"
    PREFIX
        "/"
    FILES
        ${planets-qml-images_resource_files}
)
