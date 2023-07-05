# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qshadergraphloader.pro.

#####################################################################
## tst_qshadergraphloader Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qt3d_qshadergraphloader LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# special case begin
qt_internal_add_test(tst_qt3d_qshadergraphloader
# special case end
    SOURCES
        tst_qshadergraphloader.cpp
    LIBRARIES
        Qt::3DRenderPrivate
        Qt::Gui
)
