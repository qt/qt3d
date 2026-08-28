# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(compressed_textures LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from compressed_textures.pro.

#####################################################################
## compressed_textures Binary:
#####################################################################

qt_internal_add_manual_test(compressed_textures
    GUI
    SOURCES
        main.cpp
    DEFINES
        DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DLogic
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(compressed_textures_resource_files
    "main.qml"
)

qt_internal_add_resource(compressed_textures "compressed_textures"
    PREFIX
        "/"
    FILES
        ${compressed_textures_resource_files}
)


#### Keys ignored in scope 1:.:.:compressed_textures.pro:<TRUE>:
# OTHER_FILES = "main.qml"
