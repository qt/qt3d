# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from ktxtextures.pro.

#####################################################################
## tst_ktxtextures Test:
#####################################################################

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_ktxtextures
    SOURCES
        tst_ktxtextures.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
    TESTDATA ${test_data}
)

#### Keys ignored in scope 1:.:.:ktxtextures.pro:<TRUE>:
# TEMPLATE = "app"
