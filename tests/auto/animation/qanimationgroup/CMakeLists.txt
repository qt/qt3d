# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qanimationgroup.pro.

#####################################################################
## tst_qanimationgroup Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qt3d_qanimationgroup LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# special case begin
qt_internal_add_test(tst_qt3d_qanimationgroup
# special case end
    SOURCES
        tst_qanimationgroup.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qanimationgroup.pro:<TRUE>:
# TEMPLATE = "app"
