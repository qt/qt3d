# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rhi_renderviews.pro.

#####################################################################
## tst_rhi_renderviews Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_rhi_renderviews LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_rhi_renderviews
    SOURCES
        tst_rhi_renderviews.cpp
)

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_rhi_renderviews USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/rhi/rhi.cmake)
qt3d_setup_rhi_renderer_target(tst_rhi_renderviews)

qt_internal_extend_target(tst_rhi_renderviews CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
