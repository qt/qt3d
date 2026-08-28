# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(compute-manual LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from compute-manual.pro.

#####################################################################
## compute-manual Binary:
#####################################################################

qt_internal_add_manual_test(compute-manual
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(compute-manual_resource_files
    "ComputeFrameGraph.qml"
    "ComputeMaterial.qml"
    "ParticlesScene.qml"
    "main.qml"
    "particles.comp"
    "particles.frag"
    "particles.vert"
)

qt_internal_add_resource(compute-manual "compute-manual"
    PREFIX
        "/"
    FILES
        ${compute-manual_resource_files}
)


## Scopes:
#####################################################################
