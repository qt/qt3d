# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from alignedresourcesmanagers-sse.pro.

#####################################################################
## tst_alignedresourcesmanagers-avx Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_alignedresourcesmanagers-avx LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_alignedresourcesmanagers-avx
    SOURCES
        tst_alignedresourcesmanagers-sse.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:alignedresourcesmanagers-sse.pro:<TRUE>:
# TEMPLATE = "app"
