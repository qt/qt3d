# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(loader-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from loader-qml.pro.

#####################################################################
## loader-qml Binary:
#####################################################################

qt_internal_add_manual_test(loader-qml
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
        Qt::Qml
        Qt::Quick
)

# Resources:
set(loader-qml_resource_files
    "CuboidEntity.qml"
    "SphereEntity.qml"
    "main.qml"
)

qt_internal_add_resource(loader-qml "loader-qml"
    PREFIX
        "/"
    FILES
        ${loader-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:loader-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "SphereEntity.qml" "CuboidEntity.qml"

## Scopes:
#####################################################################
