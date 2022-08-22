# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from geometryloaders.pro.

qt_feature_module_begin(
    NO_MODULE
    PUBLIC_FILE "qtgeometryloaders-config.h"
    PRIVATE_FILE "qtgeometryloaders-config_p.h"
)
include(configure.cmake)
qt_feature_module_end(NO_MODULE)

add_subdirectory(gltf)
if(QT_FEATURE_regularexpression)
    add_subdirectory(default)
endif()
if(QT_FEATURE_qt3d_fbxsdk)
    add_subdirectory(fbx)
endif()
