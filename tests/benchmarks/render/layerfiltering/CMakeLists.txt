# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from layerfiltering.pro.

#####################################################################
## tst_bench_layerfiltering Test:
#####################################################################

qt_internal_add_test(tst_bench_layerfiltering
    SOURCES
        tst_bench_layerfiltering.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:layerfiltering.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(${PROJECT_SOURCE_DIR}/tests/auto/render/commons/commons.cmake)
qt3d_setup_common_render_test(tst_bench_layerfiltering)
