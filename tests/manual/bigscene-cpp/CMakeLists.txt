# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(bigscene-cpp LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from bigscene-cpp.pro.

#####################################################################
## bigscene-cpp Binary:
#####################################################################

qt_internal_add_manual_test(bigscene-cpp
    GUI
    SOURCES
        entity.cpp entity.h
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
)

## Scopes:
#####################################################################
