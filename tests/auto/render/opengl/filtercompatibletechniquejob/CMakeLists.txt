# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from filtercompatibletechniquejob.pro.

#####################################################################
## tst_filtercompatibletechniquejob Test:
#####################################################################

qt_internal_add_test(tst_filtercompatibletechniquejob
    SOURCES
        tst_filtercompatibletechniquejob.cpp
)

#### Keys ignored in scope 1:.:.:filtercompatibletechniquejob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../commons/commons.cmake)
qt3d_setup_common_render_test(tst_filtercompatibletechniquejob USE_TEST_ASPECT)
qt3d_setup_common_test(tst_filtercompatibletechniquejob)
include(${PROJECT_SOURCE_DIR}/src/plugins/renderers/opengl/opengl.cmake)
qt3d_setup_opengl_renderer_target(tst_filtercompatibletechniquejob)

qt_internal_extend_target(tst_filtercompatibletechniquejob CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
