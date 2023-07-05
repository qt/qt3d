# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qvertexblendanimation.pro.

#####################################################################
## tst_qvertexblendanimation Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qvertexblendanimation LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qvertexblendanimation
    SOURCES
        tst_qvertexblendanimation.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qvertexblendanimation.pro:<TRUE>:
# TEMPLATE = "app"
