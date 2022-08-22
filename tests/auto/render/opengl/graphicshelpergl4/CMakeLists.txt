# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from graphicshelpergl4.pro.

#####################################################################
## tst_graphicshelpergl4 Test:
#####################################################################

qt_internal_add_test(tst_graphicshelpergl4
    SOURCES
        tst_graphicshelpergl4.cpp
)

#### Keys ignored in scope 1:.:.:graphicshelpergl4.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_graphicshelpergl4 USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_graphicshelpergl4)

qt_internal_extend_target(tst_graphicshelpergl4 CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
