# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(custom-mesh-update-data-cpp LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from custom-mesh-update-data-cpp.pro.

#####################################################################
## custom-mesh-update-data-cpp Binary:
#####################################################################

qt_internal_add_manual_test(custom-mesh-update-data-cpp
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DRender
        Qt::3DExtras
        Qt::Gui
)

## Scopes:
#####################################################################
