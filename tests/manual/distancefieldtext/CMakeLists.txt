# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(distancefieldtext LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from distancefieldtext.pro.

#####################################################################
## distancefieldtext Binary:
#####################################################################

qt_internal_add_manual_test(distancefieldtext
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
set(distancefieldtext_resource_files
    "TextScene.qml"
    "main.qml"
)

qt_internal_add_resource(distancefieldtext "distancefieldtext"
    PREFIX
        "/"
    FILES
        ${distancefieldtext_resource_files}
)


#### Keys ignored in scope 1:.:.:distancefieldtext.pro:<TRUE>:
# OTHER_FILES = "TextScene.qml" "main.qml"

## Scopes:
#####################################################################
