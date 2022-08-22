# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from materialparametergathering.pro.

#####################################################################
## tst_bench_materialparametergathering Test:
#####################################################################

qt_internal_add_test(tst_bench_materialparametergathering
    SOURCES
        tst_bench_materialparametergathering.cpp
    LIBRARIES
        Qt::3DExtras
)

#### Keys ignored in scope 1:.:.:materialparametergathering.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(${PROJECT_SOURCE_DIR}/tests/auto/render/commons/commons.cmake)
qt3d_setup_common_render_test(tst_bench_materialparametergathering USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_bench_materialparametergathering)

qt_internal_extend_target(tst_bench_materialparametergathering CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
