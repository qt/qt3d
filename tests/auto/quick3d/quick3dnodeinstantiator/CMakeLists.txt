# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3dnodeinstantiator.pro.

#####################################################################
## tst_qnodeinstantiator Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qnodeinstantiator LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_qnodeinstantiator
    SOURCES
        ../../shared/util.cpp ../../shared/util.h
        stringmodel.h
        tst_quick3dnodeinstantiator.cpp
    DEFINES
        QT_QMLTEST_DATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    INCLUDE_DIRECTORIES
        ../../shared
    LIBRARIES
        Qt::3DCore
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
    TESTDATA ${test_data}
)

#### Keys ignored in scope 1:.:.:quick3dnodeinstantiator.pro:<TRUE>:
# OTHER_FILES = "data/createAndRemove.qml" "data/createMultiple.qml" "data/createNone.qml" "data/createSingle.qml" "data/createNested.qml" "data/inactive.qml" "data/stringModel.qml"

## Scopes:
#####################################################################
