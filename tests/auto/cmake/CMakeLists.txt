# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause


cmake_minimum_required(VERSION 3.16)

project(qt3d_cmake_tests)

enable_testing()

find_package(Qt6Core REQUIRED)

include("${_Qt6CTestMacros}")

set(QAspectEngine_NAMESPACE Qt3DCore)
set(QMesh_NAMESPACE Qt3DRender)

_qt_internal_test_module_includes(
  3DCore QAspectEngine
  3DRender QMesh
)
