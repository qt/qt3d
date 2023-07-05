# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from vector4d_sse.pro.

#####################################################################
## tst_vector4d_sse Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_vector4d_sse LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_vector4d_sse
    SOURCES
        tst_vector4d_sse.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Gui
)
