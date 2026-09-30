# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from arraypolicy.pro.

#####################################################################
## tst_bench_arraypolicy Binary:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_bench_arraypolicy LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_benchmark(tst_bench_arraypolicy
    SOURCES
        tst_bench_arraypolicy.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Gui
        Qt::Test
)

#### Keys ignored in scope 1:.:.:arraypolicy.pro:<TRUE>:
# TEMPLATE = "app"
