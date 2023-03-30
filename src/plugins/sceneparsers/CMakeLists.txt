# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from sceneparsers.pro.

add_subdirectory(gltf)
if(QT_FEATURE_qt3d_assimp AND NOT QNX
        AND (CLANG OR GCC OR QT_FEATURE_qt3d_system_assimp OR (ANDROID AND CLANG) OR MSVC))
    add_subdirectory(assimp)
endif()
if(QT_FEATURE_regularexpression AND QT_FEATURE_temporaryfile AND TARGET Qt::3DExtras)
    add_subdirectory(gltfexport)
endif()
