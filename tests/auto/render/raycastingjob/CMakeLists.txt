# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from raycastingjob.pro.

#####################################################################
## tst_raycastingjob Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_raycastingjob LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_raycastingjob
    SOURCES
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_raycastingjob.cpp
    INCLUDE_DIRECTORIES
        ../qmlscenereader
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(raycastingjob_resource_files
    "testscene_screenraycasting.qml"
    "testscene_worldraycasting.qml"
    "testscene_worldraycastingalllayers.qml"
    "testscene_worldraycastinglayer.qml"
    "testscene_worldraycastingparentlayer.qml"
)

qt_internal_add_resource(tst_raycastingjob "raycastingjob"
    PREFIX
        "/"
    FILES
        ${raycastingjob_resource_files}
)


#### Keys ignored in scope 1:.:.:raycastingjob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_raycastingjob)
