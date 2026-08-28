# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(subtree-enabler-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from subtree-enabler-qml.pro.

#####################################################################
## subtree-enabler-qml Binary:
#####################################################################

qt_internal_add_manual_test(subtree-enabler-qml
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
set(subtree-enabler-qml_resource_files
    "FrameGraph.qml"
    "SimpleCamera.qml"
    "main.qml"
)

qt_internal_add_resource(subtree-enabler-qml "subtree-enabler-qml"
    PREFIX
        "/"
    FILES
        ${subtree-enabler-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:subtree-enabler-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
