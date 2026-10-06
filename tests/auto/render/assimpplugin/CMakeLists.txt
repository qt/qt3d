# Copyright (C) 2026 Klaralvdalens Datakonsult AB (KDAB).
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_assimpplugin LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_assimpplugin
    SOURCES
        tst_assimpplugin.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::3DAnimation
        Qt::Gui
)

set(tst_assimpplugin_resource_files
    "triangle.gltf"
    "morph_target.gltf"
    "mixed_morph_target_attribs.gltf"
)

qt_internal_add_resource(tst_assimpplugin "assimpplugin"
    PREFIX
        "/"
    FILES
        ${tst_assimpplugin_resource_files}
)
