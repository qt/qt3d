# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(scene3d-visibility LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from scene3d-visibility.pro.

#####################################################################
## scene3d-visibility Binary:
#####################################################################

qt_internal_add_manual_test(scene3d-visibility
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DInput
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(scene3d-visibility_resource_files
    "main.qml"
)

qt_internal_add_resource(scene3d-visibility "scene3d-visibility"
    PREFIX
        "/"
    FILES
        ${scene3d-visibility_resource_files}
)


#### Keys ignored in scope 1:.:.:scene3d-visibility.pro:<TRUE>:
# OTHER_FILES = "main.qml"
