# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

include(${PROJECT_SOURCE_DIR}/tests/auto/core/common/common.cmake)

function(qt3d_setup_common_render_test target)
    cmake_parse_arguments(args "USE_TEST_ASPECT" "" "" ${ARGN})
    set(commonDir ${PROJECT_SOURCE_DIR}/tests/auto/core/common)
    set(commonsDir ${PROJECT_SOURCE_DIR}/tests/auto/render/commons)
    target_sources(${target} PRIVATE ${commonsDir}/testrenderer.h ${commonsDir}/testrenderer.cpp)
    if (args_USE_TEST_ASPECT)
        target_sources(${target} PRIVATE ${commonsDir}/testaspect.h ${commonsDir}/testaspect.cpp)
    endif()
    target_include_directories(${target} PRIVATE ${commonsDir})

    qt3d_setup_common_test(${target})
endfunction()
