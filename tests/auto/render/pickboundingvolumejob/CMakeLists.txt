# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from pickboundingvolumejob.pro.

#####################################################################
## tst_pickboundingvolumejob Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_pickboundingvolumejob LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_pickboundingvolumejob
    SOURCES
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_pickboundingvolumejob.cpp
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
set(pickboundingvolumejob_resource_files
    "testscene_cameraposition.qml"
    "testscene_childentity.qml"
    "testscene_dragdisabled.qml"
    "testscene_dragenabled.qml"
    "testscene_dragenabledhoverenabled.qml"
    "testscene_dragenabledoverlapping.qml"
    "testscene_improperframegraph.qml"
    "testscene_nopicking.qml"
    "testscene_parententity.qml"
    "testscene_pickersdisabled.qml"
    "testscene_priorityoverlapping.qml"
    "testscene_viewports.qml"
    "testscene_layerfilter.qml"
    "testscene_nested_layerfilter.qml"
)

qt_internal_add_resource(tst_pickboundingvolumejob "pickboundingvolumejob"
    PREFIX
        "/"
    FILES
        ${pickboundingvolumejob_resource_files}
)


#### Keys ignored in scope 1:.:.:pickboundingvolumejob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_pickboundingvolumejob)
