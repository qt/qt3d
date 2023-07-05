# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qtext2dentity.pro.

#####################################################################
## tst_qtext2dentity Test:
#####################################################################

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qtext2dentity LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qtext2dentity
    SOURCES
        tst_qtext2dentity.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DQuickExtras
        Qt::Gui
        Qt::Qml
)

# Resources:
set(qtext2dentity_resource_files
    "qtext2dentity.qml"
)

qt_internal_add_resource(tst_qtext2dentity "qtext2dentity"
    PREFIX
        "/"
    FILES
        ${qtext2dentity_resource_files}
)


#### Keys ignored in scope 1:.:.:qtext2dentity.pro:<TRUE>:
# TEMPLATE = "app"
