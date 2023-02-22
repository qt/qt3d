# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(widgets-scene3d LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 3DInput Core Gui Qml Quick QuickWidgets Widgets)

qt_add_executable(widgets-scene3d
    main.cpp
)

set_target_properties(widgets-scene3d PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(widgets-scene3d PUBLIC
    Qt::3DInput
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickWidgets
    Qt::Widgets
)

# Resources:
set_source_files_properties("../scene3d/AnimatedEntity.qml"
    PROPERTIES QT_RESOURCE_ALIAS "AnimatedEntity.qml"
)

set_source_files_properties("../scene3d/main.qml"
    PROPERTIES QT_RESOURCE_ALIAS "main.qml"
)

set(widgets-scene3d_resource_files
    "../scene3d/AnimatedEntity.qml"
    "../scene3d/main.qml"
)

qt6_add_resources(widgets-scene3d "widgets-scene3d"
    PREFIX
        "/"
    FILES
        ${widgets-scene3d_resource_files}
)
