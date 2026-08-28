# Copyright (C) 2022 Klaralvdalens Datakonsult AB (KDAB).
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(multi-qquickwidgets-scene3d LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

#####################################################################
## multi-qquickwidgets-scene3d Binary:
#####################################################################

qt_internal_add_manual_test(multi-qquickwidgets-scene3d
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
        Qt::Quick
        Qt::Widgets
        Qt::QuickWidgets
)

# Resources:
set(multi-qquickwidgets-scene3d_resource_files
    "main.qml"
    "main2.qml"
)

qt_internal_add_resource(multi-qquickwidgets-scene3d "multi-qquickwidgets-scene3d"
    PREFIX
        "/"
    FILES
        ${multi-qquickwidgets-scene3d_resource_files}
)
