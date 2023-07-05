# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3dbuffer.pro.

#####################################################################
## tst_quick3dbuffer Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_quick3dbuffer LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_quick3dbuffer
    SOURCES
        tst_quick3dbuffer.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:quick3dbuffer.pro:<TRUE>:
# TEMPLATE = "app"
