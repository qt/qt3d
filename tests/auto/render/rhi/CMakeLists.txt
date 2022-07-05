# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from rhi.pro.

if (NOT QT_BUILD_STANDALONE_TESTS)
    add_subdirectory(rhi_renderviews)
    add_subdirectory(rhi_rendercommands)
    add_subdirectory(rhi_graphicspipelinemanager)
endif()
