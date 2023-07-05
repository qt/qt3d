# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from updateshaderdatatransformjob.pro.

#####################################################################
## tst_updateshaderdatatransformjob Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_updateshaderdatatransformjob LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_updateshaderdatatransformjob
    SOURCES
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_updateshaderdatatransformjob.cpp
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
set(updateshaderdatatransformjob_resource_files
    "test_scene_model_to_eye.qml"
    "test_scene_model_to_world.qml"
)

qt_internal_add_resource(tst_updateshaderdatatransformjob "updateshaderdatatransformjob"
    PREFIX
        "/"
    FILES
        ${updateshaderdatatransformjob_resource_files}
)


#### Keys ignored in scope 1:.:.:updateshaderdatatransformjob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_updateshaderdatatransformjob)
