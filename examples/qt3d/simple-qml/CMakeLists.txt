# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(simple-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DLogic 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(simple-qml
    main.cpp
)

set_target_properties(simple-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(simple-qml PUBLIC
    Qt::3DCore
    Qt::3DInput
    Qt::3DLogic
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(simple-qml_resource_files
    "main.qml"
)

qt6_add_resources(simple-qml "simple-qml"
    PREFIX
        "/"
    FILES
        ${simple-qml_resource_files}
)

target_sources(simple-qml PRIVATE ${simple-qml_resource_files})

install(TARGETS simple-qml
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
