# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause



#### Inputs

# input qt3d-assimp
set(INPUT_qt3d_assimp "undefined" CACHE STRING "")
set_property(CACHE INPUT_qt3d_assimp PROPERTY STRINGS undefined qt system no)



#### Libraries

qt_find_package(WrapQt3DAssimp 5 PROVIDED_TARGETS WrapQt3DAssimp::WrapQt3DAssimp MODULE_NAME 3dcore QMAKE_LIB qt3d-assimp)

qt_config_compile_test("assimp"
                   LABEL ""
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../config.tests/assimp"
)


#### Tests



#### Features

qt_feature("qt3d-assimp" PUBLIC PRIVATE
    LABEL "Assimp"
)
qt_feature_definition("qt3d-assimp" "QT_NO_QT3D_ASSIMP" NEGATE VALUE "1")
qt_feature("qt3d-system-assimp" PRIVATE
    LABEL "System Assimp"
    CONDITION QT_FEATURE_qt3d_assimp AND TEST_assimp
    ENABLE INPUT_assimp STREQUAL 'system'
    DISABLE INPUT_assimp STREQUAL 'qt'
)
qt_feature("qt3d-render" PUBLIC
    SECTION "Aspects"
    LABEL "Render aspect"
    PURPOSE "Use the 3D Render Aspect library"
)
qt_feature("qt3d-input" PUBLIC
    SECTION "Aspects"
    LABEL "Input aspect"
    PURPOSE "Use the 3D Input Aspect library"
)
qt_feature("qt3d-logic" PUBLIC
    SECTION "Aspects"
    LABEL "Logic aspect"
    PURPOSE "Use the 3D Logic Aspect library"
)
qt_feature("qt3d-extras" PUBLIC
    SECTION "Aspects"
    LABEL "Extras aspect"
    PURPOSE "Use the 3D Extra library"
    CONDITION QT_FEATURE_qt3d_render AND QT_FEATURE_qt3d_input AND QT_FEATURE_qt3d_logic
)
qt_feature("qt3d-animation" PUBLIC
    SECTION "Aspects"
    LABEL "Animation aspect"
    PURPOSE "Use the 3D Animation Aspect library"
    CONDITION QT_FEATURE_qt3d_render
)
qt_feature("qt3d-simd-sse2" PRIVATE
    LABEL "Use SSE2 instructions"
    PURPOSE "Use SSE2 SIMD instructions to accelerate matrix operations"
    AUTODETECT QT_FEATURE_sse2
    ENABLE INPUT_qt3d_simd STREQUAL 'sse2' OR INPUT_qt3d_simd STREQUAL 'avx2'
    DISABLE INPUT_qt3d_simd STREQUAL 'no' OR ( TEST_architecture_arch STREQUAL i386 )
)
qt_feature("qt3d-simd-avx2" PRIVATE
    LABEL "Use AVX2 instructions"
    PURPOSE "Use AVX2 SIMD instructions to accelerate matrix operations"
    AUTODETECT TEST_arch_${TEST_architecture_arch}_subarch_avx2
    ENABLE INPUT_qt3d_simd STREQUAL 'avx2'
    DISABLE INPUT_qt3d_simd STREQUAL 'sse2' OR INPUT_qt3d_simd STREQUAL 'no' OR ( TEST_architecture_arch STREQUAL i386 )
)
qt_configure_add_summary_section(NAME "Qt 3D")
qt_configure_add_summary_entry(ARGS "qt3d-assimp")
qt_configure_add_summary_entry(ARGS "qt3d-system-assimp")
qt_configure_add_summary_entry(ARGS "qt3d-simd-sse2")
qt_configure_add_summary_entry(ARGS "qt3d-simd-avx2")
qt_configure_add_summary_section(NAME "Aspects")
qt_configure_add_summary_entry(ARGS "qt3d-render")
qt_configure_add_summary_entry(ARGS "qt3d-input")
qt_configure_add_summary_entry(ARGS "qt3d-logic")
qt_configure_add_summary_entry(ARGS "qt3d-animation")
qt_configure_add_summary_entry(ARGS "qt3d-extras")
qt_configure_end_summary_section() # end of "Aspects" section
qt_configure_end_summary_section() # end of "Qt 3D" section
