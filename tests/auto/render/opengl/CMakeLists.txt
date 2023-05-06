# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from opengl.pro.

if (NOT QT_BUILD_STANDALONE_TESTS)
    add_subdirectory(filtercompatibletechniquejob)
if (NOT ANDROID)
    add_subdirectory(graphicshelpergl3_3)
    add_subdirectory(graphicshelpergl3_2)
    add_subdirectory(graphicshelpergl2)
endif()
    add_subdirectory(glshadermanager)
    add_subdirectory(textures)
    add_subdirectory(renderer)
    add_subdirectory(renderviewutils)
    add_subdirectory(renderviews)
    add_subdirectory(renderqueue)
    add_subdirectory(renderviewbuilder)
    add_subdirectory(qgraphicsutils)
    add_subdirectory(computecommand)
    add_subdirectory(gltexturemanager)
    if(TARGET Qt::Quick)
        add_subdirectory(materialparametergathererjob)
    endif()
    if(NOT MACOS AND NOT ANDROID)
        add_subdirectory(graphicshelpergl4)
    endif()
endif()
