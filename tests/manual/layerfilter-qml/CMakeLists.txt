# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(layerfilter-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from layerfilter-qml.pro.

#####################################################################
## layerfilter-qml Binary:
#####################################################################

qt_internal_add_manual_test(layerfilter-qml
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
set(layerfilter-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(layerfilter-qml "layerfilter-qml"
    PREFIX
        "/"
    FILES
        ${layerfilter-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:layerfilter-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
