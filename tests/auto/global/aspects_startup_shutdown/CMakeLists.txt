# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from aspects_startup_shutdown.pro.

#####################################################################
## tst_aspects_startup_shutdown Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_aspects_startup_shutdown LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_aspects_startup_shutdown
    SOURCES
        tst_aspects_startup_shutdown.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:aspects_startup_shutdown.pro:<TRUE>:
# TEMPLATE = "app"
