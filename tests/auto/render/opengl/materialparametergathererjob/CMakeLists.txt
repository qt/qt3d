# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from materialparametergathererjob.pro.

#####################################################################
## tst_materialparametergathererjob Test:
#####################################################################

qt_internal_add_test(tst_materialparametergathererjob
    SOURCES
        tst_materialparametergathererjob.cpp
)

#### Keys ignored in scope 1:.:.:materialparametergathererjob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_materialparametergathererjob)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_materialparametergathererjob)

qt_internal_extend_target(tst_materialparametergathererjob CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
