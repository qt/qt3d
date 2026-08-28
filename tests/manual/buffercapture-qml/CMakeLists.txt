# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(buffercapture-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from buffercapture-qml.pro.

#####################################################################
## buffercapture-qml Binary:
#####################################################################

qt_internal_add_manual_test(buffercapture-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DLogic
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(resources_resource_files
    "BufferSetterScene.qml"
    "ComputeFrameGraph.qml"
    "ComputeMaterial.qml"
    "gl43/bufferSetter.comp"
    "gl45/bufferSetter.comp"
    "main.qml"
)

qt_internal_add_resource(buffercapture-qml "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)


#### Keys ignored in scope 1:.:.:buffercapture-qml.pro:<TRUE>:
# DISTFILES = "main.qml" "main.qml"

## Scopes:
#####################################################################
