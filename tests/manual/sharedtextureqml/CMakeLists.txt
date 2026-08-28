# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(sharedtextureqml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from sharedtextureqml.pro.

#####################################################################
## sharedtextureqml Binary:
#####################################################################

qt_internal_add_manual_test(sharedtextureqml
    GUI
    SOURCES
        ../sharedtexture/videoplayer.cpp ../sharedtexture/videoplayer.h
        main.cpp
    INCLUDE_DIRECTORIES
        ../sharedtexture
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::GuiPrivate
        Qt::Multimedia
        Qt::Quick
        Qt::Widgets
)

# Resources:
set(qml_resource_files
    "main.qml"
)

qt_internal_add_resource(sharedtextureqml "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)


## Scopes:
#####################################################################
