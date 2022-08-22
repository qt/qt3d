# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(qardboard LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DExtras 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(qardboard
    abstractdeviceorientation.cpp abstractdeviceorientation.h
    dummydeviceorientation.cpp dummydeviceorientation.h
    main.cpp
)

set_target_properties(qardboard PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qardboard PUBLIC
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
    "PlacedEntity.qml"
    "QardboardCamera.qml"
    "QardboardFrameGraph.qml"
    "QardboardRootEntity.qml"
    "QardboardScene3D.qml"
    "main.qml"
)

qt6_add_resources(qardboard "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)

set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt6_add_resources(qardboard "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)

if(IOS)
    target_sources(qardboard PUBLIC
        iosdeviceorientation.h iosdeviceorientation.mm iosdeviceorientation_p.h iosdeviceorientation_p.mm
    )

    target_link_libraries(qardboard PUBLIC
        Qt::Sensors
        Qt::SensorsPrivate
    )
endif()

install(TARGETS qardboard
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
