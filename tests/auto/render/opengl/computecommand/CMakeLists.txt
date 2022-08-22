# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from computecommand.pro.

#####################################################################
## tst_computecommand Test:
#####################################################################

qt_internal_add_test(tst_computecommandgl
    SOURCES
        tst_computecommand.cpp
)

#### Keys ignored in scope 1:.:.:computecommand.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_computecommandgl USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_computecommandgl)

qt_internal_extend_target(tst_computecommandgl CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
