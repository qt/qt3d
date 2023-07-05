# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qshaderprogram.pro.

#####################################################################
## tst_qshaderprogram Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qshaderprogram LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qshaderprogram
    SOURCES
        tst_qshaderprogram.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

# Resources:
set(shaders_resource_files
    "included.frag"
    "main.frag"
    "mainabsolute.frag"
)

qt_internal_add_resource(tst_qshaderprogram "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)


#### Keys ignored in scope 1:.:.:qshaderprogram.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_qshaderprogram)
