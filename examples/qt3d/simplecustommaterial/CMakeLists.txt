# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(simplecustommaterial LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DRender Core Gui Qml Quick)

qt_add_executable(simplecustommaterial
    main.cpp
)

set_target_properties(simplecustommaterial PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(simplecustommaterial PUBLIC
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "PlaneModel.qml"
    "SceneRoot.qml"
    "SimpleMaterial.qml"
    "main.qml"
)

qt6_add_resources(simplecustommaterial "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

set(shaders_resource_files
    "shaders/es2/simpleColor.frag"
    "shaders/es2/simpleColor.vert"
    "shaders/gl3/simpleColor.frag"
    "shaders/gl3/simpleColor.vert"
    "shaders/gl45/simpleColor.frag"
    "shaders/gl45/simpleColor.vert"
)

qt6_add_resources(simplecustommaterial "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)

install(TARGETS simplecustommaterial
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
