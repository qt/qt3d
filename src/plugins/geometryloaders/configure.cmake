# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#### Inputs

# input fbxsdk
set(INPUT_fbxsdk "undefined" CACHE STRING "")
set_property(CACHE INPUT_fbxsdk PROPERTY STRINGS undefined no system)


#### Libraries

#qt_find_package(WrapAssimp 5 PROVIDED_TARGETS WrapAssimp::WrapAssimp)

qt_config_compile_test("qt3d_fbxsdk"
                   LABEL "Autodesk FBX"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../config.tests/fbx"
#                   LIBRARIES WrapAssimp::WrapAssimp
#                   PACKAGES PACKAGE WrapAssimp 5
                   )
#### Tests



#### Features

qt_feature("qt3d_fbxsdk" PRIVATE
    LABEL "Autodesk FBX"
    CONDITION libs.fbx #OR FIXME
)
qt_feature_definition("qt3d_fbxsdk" "QT_3DGEOMETRYLOADERS_FBX")

qt_configure_add_summary_section(NAME "Qt3D Geometry Loaders")
qt_configure_add_summary_entry(ARGS "qt3d_fbxsdk")
qt_configure_end_summary_section() # end of "Qt3D" section
