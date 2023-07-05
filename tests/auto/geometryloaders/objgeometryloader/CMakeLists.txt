# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from objgeometryloader.pro.

#####################################################################
## tst_objgeometryloader Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_objgeometryloader LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_objgeometryloader
    SOURCES
        tst_objgeometryloader.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

# Resources:
set(resources_resource_files
    "invalid_vertex_position.obj"
)

qt_internal_add_resource(tst_objgeometryloader "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)


#### Keys ignored in scope 1:.:.:objgeometryloader.pro:<TRUE>:
# OTHER_FILES = "invalid_vertex_position.obj"
# TEMPLATE = "app"
