# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from genericlambdajob.pro.

#####################################################################
## tst_genericlambdajob Test:
#####################################################################

qt_internal_add_test(tst_genericlambdajob
    SOURCES
        tst_genericlambdajob.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:genericlambdajob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_genericlambdajob USE_TEST_ASPECT)
