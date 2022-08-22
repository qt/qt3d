# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from renderviewutils.pro.

#####################################################################
## tst_renderviewutils Test:
#####################################################################

qt_internal_add_test(tst_renderviewutils
    SOURCES
        tst_renderviewutils.cpp
)

#### Keys ignored in scope 1:.:.:renderviewutils.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_renderviewutils USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_renderviewutils)

qt_internal_extend_target(tst_renderviewutils CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
