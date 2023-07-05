# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from Qt::3DCore.pro.

#####################################################################
## tst_import3dcore Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_import3dcore LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_import3dcore
    SOURCES
        ../../render/qmlscenereader/qmlscenereader.cpp ../../render/qmlscenereader/qmlscenereader.h
        tst_import3dcore.cpp
    INCLUDE_DIRECTORIES
        ../../render/qmlscenereader
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

# Resources:
set(3dcore_resource_files
    "3dcore.qml"
)

qt_internal_add_resource(tst_import3dcore "3dcore"
    PREFIX
        "/"
    FILES
        ${3dcore_resource_files}
)


#### Keys ignored in scope 1:.:.:3dcore.pro:<TRUE>:
# TEMPLATE = "app"
