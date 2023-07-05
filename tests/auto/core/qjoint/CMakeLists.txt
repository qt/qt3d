# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qjoint.pro.

#####################################################################
## tst_qjoint Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qjoint LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qjoint
    SOURCES
        tst_qjoint.cpp
    INCLUDE_DIRECTORIES
        ../common
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qjoint.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qjoint CONDITION QT_FEATURE_private_tests
    SOURCES
        ../common/qbackendnodetester.cpp ../common/qbackendnodetester.h
        ../common/testarbiter.h
)
