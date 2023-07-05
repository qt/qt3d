# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from dynamicnodecreation.pro.

#####################################################################
## tst_dynamicnodecreation Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_dynamicnodecreation LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_dynamicnodecreation
    SOURCES
        ../../shared/util.cpp ../../shared/util.h
        tst_dynamicnodecreation.cpp
    DEFINES
        QT_QMLTEST_DATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    INCLUDE_DIRECTORIES
        ../../shared
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
    TESTDATA ${test_data}
)

#### Keys ignored in scope 1:.:.:dynamicnodecreation.pro:<TRUE>:
# OTHER_FILES = "data/createDynamicChild.qml" "data/createMultiple.qml" "data/createSingle.qml" "data/inactive.qml" "data/dynamicEntity.qml"

## Scopes:
#####################################################################
