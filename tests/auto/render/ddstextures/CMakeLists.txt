# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from ddstextures.pro.

#####################################################################
## tst_ddstextures Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_ddstextures LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_ddstextures
    SOURCES
        tst_ddstextures.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
    TESTDATA ${test_data}
)

#### Keys ignored in scope 1:.:.:ddstextures.pro:<TRUE>:
# OTHER_FILES = "data/16x16x1-1-bc1.dds" "data/16x16x1-1-bc1-dx10.dds" "data/16x16x1-1-bc1-nomips.dds" "data/16x16x1-1-bc1-nomips-dx10.dds" "data/16x16x1-1-bc3.dds" "data/16x16x1-1-bc3-dx10.dds" "data/16x16x1-1-bc3-nomips.dds" "data/16x16x1-1-bc3-nomips-dx10.dds" "data/16x16x1-1-lumi.dds" "data/16x16x1-1-lumi-nomips.dds" "data/16x16x1-1-rgb.dds" "data/16x16x1-1-rgb-nomips.dds" "data/16x16x1-6-bc1.dds" "data/16x16x1-6-bc1-dx10.dds" "data/16x16x1-6-bc1-nomips.dds" "data/16x16x1-6-bc1-nomips-dx10.dds" "data/16x16x1-6-bc3.dds" "data/16x16x1-6-bc3-dx10.dds" "data/16x16x1-6-bc3-nomips.dds" "data/16x16x1-6-bc3-nomips-dx10.dds" "data/16x16x1-6-lumi.dds" "data/16x16x1-6-lumi-nomips.dds" "data/16x16x1-6-rgb.dds" "data/16x16x1-6-rgb-nomips.dds" "data/16x16-etc1.pkm" "data/16x16-etc2.pkm"
# TEMPLATE = "app"
