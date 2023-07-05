# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from animationutils.pro.

#####################################################################
## tst_animationutils Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_animationutils LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_animationutils
    SOURCES
        tst_animationutils.cpp
    LIBRARIES
        Qt::3DAnimation
        Qt::3DAnimationPrivate
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

# Resources:
set(animationutils_resource_files
    "clip1.json"
    "clip2.json"
    "clip3.json"
    "clip4.json"
    "clip5.json"
    "clip6.json"
)

qt_internal_add_resource(tst_animationutils "animationutils"
    PREFIX
        "/"
    FILES
        ${animationutils_resource_files}
)


#### Keys ignored in scope 1:.:.:animationutils.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_animationutils)
