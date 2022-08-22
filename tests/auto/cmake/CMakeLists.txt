# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause


cmake_minimum_required(VERSION 3.16)

project(qmake_cmake_files)

enable_testing()

find_package(Qt5Core REQUIRED)

include("${_Qt5CTestMacros}")

set(QAspectEngine_NAMESPACE Qt3DCore)
set(QMesh_NAMESPACE Qt3DRender)

test_module_includes(
  3DCore QAspectEngine
  3DRender QMesh
)
