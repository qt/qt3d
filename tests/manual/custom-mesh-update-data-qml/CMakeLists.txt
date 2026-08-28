# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(custom-mesh-update-data-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from custom-mesh-update-data-qml.pro.

#####################################################################
## custom-mesh-update-data-qml Binary:
#####################################################################

qt_internal_add_manual_test(custom-mesh-update-data-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Quick
)

# Resources:
set(custom-mesh-update-data-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(custom-mesh-update-data-qml "custom-mesh-update-data-qml"
    PREFIX
        "/"
    FILES
        ${custom-mesh-update-data-qml_resource_files}
)


## Scopes:
#####################################################################
