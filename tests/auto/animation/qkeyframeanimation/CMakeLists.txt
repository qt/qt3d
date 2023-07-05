# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qkeyframeanimation.pro.

#####################################################################
## tst_qkeyframeanimation Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qkeyframeanimation LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qkeyframeanimation
    SOURCES
        tst_qkeyframeanimation.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qkeyframeanimation.pro:<TRUE>:
# TEMPLATE = "app"
