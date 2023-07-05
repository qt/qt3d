# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from threadpooler.pro.

#####################################################################
## tst_threadpooler Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_threadpooler LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_threadpooler
    SOURCES
        tst_threadpooler.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:threadpooler.pro:<TRUE>:
# TEMPLATE = "app"
