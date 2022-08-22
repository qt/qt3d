# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from jobs.pro.

#####################################################################
## tst_bench_jobs Binary:
#####################################################################

qt_internal_add_benchmark(tst_bench_jobs
    SOURCES
        tst_bench_jobs.cpp
    DEFINES
        QT3D_RENDER_UNIT_TESTS
    LIBRARIES
        Qt::Qml
        Qt::Test
        Qt::3DExtras
)

#### Keys ignored in scope 1:.:.:jobs.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(${PROJECT_SOURCE_DIR}/tests/auto/render/commons/commons.cmake)
qt3d_setup_common_render_test(tst_bench_jobs USE_TEST_ASPECT)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_bench_jobs)

qt_internal_extend_target(tst_bench_jobs CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
