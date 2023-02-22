# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(shadow-map-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(shadow-map-qml
    main.cpp
)

set_target_properties(shadow-map-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(shadow-map-qml PUBLIC
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
set(shadow-map-qml_resource_files
    "AdsEffect.qml"
    "AdsMaterial.qml"
    "GroundPlane.qml"
    "ShadowMapFrameGraph.qml"
    "ShadowMapLight.qml"
    "Toyplane.qml"
    "Trefoil.qml"
    "main.qml"
    "shaders/ads.frag"
    "shaders/ads.vert"
    "shaders/es3/ads.frag"
    "shaders/es3/ads.vert"
    "shaders/es3/shadowmap.frag"
    "shaders/es3/shadowmap.vert"
    "shaders/rhi/ads.frag"
    "shaders/rhi/ads.vert"
    "shaders/rhi/shadowmap.frag"
    "shaders/rhi/shadowmap.vert"
    "shaders/shadowmap.frag"
    "shaders/shadowmap.vert"
)

qt6_add_resources(shadow-map-qml "shadow-map-qml"
    PREFIX
        "/"
    FILES
        ${shadow-map-qml_resource_files}
)

set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt6_add_resources(shadow-map-qml "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)

install(TARGETS shadow-map-qml
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
