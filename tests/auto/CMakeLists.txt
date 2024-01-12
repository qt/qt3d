# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from auto.pro.

if(NOT QT_FEATURE_qt3d_extras)
    return()
endif()

add_subdirectory(cmake)
add_subdirectory(core)
add_subdirectory(global)

if(QT_FEATURE_qt3d_render)
    add_subdirectory(render)
    add_subdirectory(geometryloaders)
endif()
if(QT_FEATURE_qt3d_input)
    add_subdirectory(input)
endif()
if(QT_FEATURE_qt3d_animation)
    add_subdirectory(animation)
endif()
add_subdirectory(extras)
if(QT_FEATURE_qt3d_input AND QT_FEATURE_qt3d_render AND TARGET Qt::Quick)
    add_subdirectory(quick3d)
endif()
