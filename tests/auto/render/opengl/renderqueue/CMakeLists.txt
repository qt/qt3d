# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from renderqueue.pro.

#####################################################################
## tst_renderqueue Test:
#####################################################################

qt_internal_add_test(tst_renderqueue
    SOURCES
        tst_renderqueue.cpp
)

#### Keys ignored in scope 1:.:.:renderqueue.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_renderqueue USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_renderqueue)

qt_internal_extend_target(tst_renderqueue CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
