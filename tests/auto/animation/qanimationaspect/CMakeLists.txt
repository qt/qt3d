# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qanimationaspect.pro.

#####################################################################
## tst_qanimationaspect Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qanimationaspect LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qanimationaspect
    SOURCES
        tst_qanimationaspect.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qanimationaspect.pro:<TRUE>:
# TEMPLATE = "app"
