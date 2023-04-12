# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qt3d.pro.

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(Qt3D # special case
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt 3D Libraries" # special case
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

# special case begin
find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core)
find_package(Qt6 ${PROJECT_VERSION} QUIET CONFIG OPTIONAL_COMPONENTS
    Network Concurrent Widgets Qml Quick ShaderTools Gamepad OpenGL Multimedia QuickWidgets)
# special case end

qt_internal_project_setup()

if(NOT TARGET Qt::Gui)
    message(NOTICE "Skipping the build as the condition \"TARGET Qt::Gui\" is not met.")
    return()
endif()
if(NOT TARGET Qt::Network)
    message(NOTICE "Skipping the build as the condition \"TARGET Qt::Network\" is not met.")
    return()
endif()
if(WASM OR NOT QT_FEATURE_opengl)
    message(NOTICE "Skipping the build as the condition \"QT_FEATURE_opengl AND NOT WASM\" is not met.")
    return()
endif()

# special case begin
# TODO: Fix warnings and remove this (can't remove now because of QTBUG-99248)
set(QT_REPO_NOT_WARNINGS_CLEAN TRUE)
# special case end

qt_build_repo()
