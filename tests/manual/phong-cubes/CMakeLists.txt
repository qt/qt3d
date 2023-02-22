# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(phong-cubes LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(phong-cubes
    main.cpp
)

set_target_properties(phong-cubes PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(phong-cubes PUBLIC
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
set(phong-cubes_resource_files
    "CubeEntity.qml"
    "main.qml"
)

qt6_add_resources(phong-cubes "phong-cubes"
    PREFIX
        "/"
    FILES
        ${phong-cubes_resource_files}
)

qt6_add_resources(big_resources ../exampleresources/textures.qrc)
target_sources(phong-cubes PRIVATE ${big_resources})

install(TARGETS phong-cubes
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
