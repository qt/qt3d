# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(instanced-arrays-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(instanced-arrays-qml
    instancebuffer.cpp instancebuffer.h
    main.cpp
)

set_target_properties(instanced-arrays-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(instanced-arrays-qml PUBLIC
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
set(instanced-arrays-qml_resource_files
    "gl3/instanced.frag"
    "gl3/instanced.vert"
    "gl45/instanced.frag"
    "gl45/instanced.vert"
    "main.qml"
)

qt6_add_resources(instanced-arrays-qml "instanced-arrays-qml"
    PREFIX
        "/"
    FILES
        ${instanced-arrays-qml_resource_files}
)

install(TARGETS instanced-arrays-qml
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
