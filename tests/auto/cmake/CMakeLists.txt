
cmake_minimum_required(VERSION 2.8)

project(qmake_cmake_files)

enable_testing()

find_package(Qt5Core REQUIRED)

include("${_Qt5CTestMacros}")

set(QAspectEngine_NAMESPACE Qt3D)
set(QMesh_NAMESPACE Qt3D)

test_module_includes(
  3DCore QAspectEngine
  3DRenderer QMesh
)
