# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from renderers.pro.

qt_feature_module_begin(
    NO_MODULE
    PUBLIC_FILE "qtrenderers-config.h"
    PRIVATE_FILE "qtrenderers-config_p.h"
)
include(configure.cmake)
qt_feature_module_end(NO_MODULE)

if(QT_FEATURE_qt3d_opengl_renderer)
    add_subdirectory(opengl)
endif()
if(QT_FEATURE_qt3d_rhi_renderer)
    add_subdirectory(rhi)
endif()
