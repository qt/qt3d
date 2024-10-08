# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_quick3dscene3d LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_quick3dscene3d
    SOURCES
        tst_quick3dscene3d.cpp
    LIBRARIES
        Qt::Core
        Qt::QuickPrivate
        Qt::3DQuickScene3D
        Qt::3DQuickScene3DPrivate
        Qt::Test
)
