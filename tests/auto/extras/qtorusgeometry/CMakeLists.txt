# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qtorusgeometry.pro.

#####################################################################
## tst_qtorusgeometry Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qtorusgeometry LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qtorusgeometry
    SOURCES
        ../common/geometrytesthelper.h
        tst_qtorusgeometry.cpp
    INCLUDE_DIRECTORIES
        ../common
    LIBRARIES
        Qt::3DExtras
        Qt::3DRender
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qtorusgeometry.pro:<TRUE>:
# TEMPLATE = "app"
