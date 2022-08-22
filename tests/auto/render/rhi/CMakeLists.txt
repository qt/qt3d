# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rhi.pro.

if (NOT QT_BUILD_STANDALONE_TESTS)
    add_subdirectory(rhi_renderviews)
    add_subdirectory(rhi_rendercommands)
    add_subdirectory(rhi_graphicspipelinemanager)
endif()
