# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from geometryloaders.pro.

#####################################################################
## tst_geometryloaders Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_geometryloaders LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_geometryloaders
    SOURCES
        tst_geometryloaders.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

# Resources:
set(geometryloaders_resource_files
    "cube.fbx"
    "cube.gltf"
    "cube.obj"
    "cube.ply"
    "cube.stl"
    "cube2.obj"
    "cube_buffer.bin"
)

qt_internal_add_resource(tst_geometryloaders "geometryloaders"
    PREFIX
        "/"
    FILES
        ${geometryloaders_resource_files}
)


#### Keys ignored in scope 1:.:.:geometryloaders.pro:<TRUE>:
# TEMPLATE = "app"
