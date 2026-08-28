# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(bigmodel-qml LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

# Generated from bigmodel-qml.pro.

#####################################################################
## bigmodel-qml Binary:
#####################################################################

qt_internal_add_manual_test(bigmodel-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(bigmodel-qml_resource_files
    "MyEntity.qml"
    "main.qml"
)

qt_internal_add_resource(bigmodel-qml "bigmodel-qml"
    PREFIX
        "/"
    FILES
        ${bigmodel-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:bigmodel-qml.pro:<TRUE>:
# DISTFILES = "MyEntity.qml"
# OTHER_FILES = "main.qml" "MyEntity.qml"

## Scopes:
#####################################################################
