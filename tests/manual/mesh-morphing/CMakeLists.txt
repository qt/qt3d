# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(mesh-morphing LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from mesh-morphing.pro.

#####################################################################
## mesh-morphing Binary:
#####################################################################

qt_internal_add_manual_test(mesh-morphing
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickAnimation
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

## Scopes:
#####################################################################
