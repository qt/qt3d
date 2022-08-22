# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from renderviewbuilder.pro.

#####################################################################
## tst_renderviewbuilder Test:
#####################################################################

qt_internal_add_test(tst_renderviewbuilder
    SOURCES
        tst_renderviewbuilder.cpp
)

#### Keys ignored in scope 1:.:.:renderviewbuilder.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_renderviewbuilder)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_renderviewbuilder)

qt_internal_extend_target(tst_renderviewbuilder CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
