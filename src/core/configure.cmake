#### Inputs

# input assimp
set(INPUT_assimp "undefined" CACHE STRING "")
set_property(CACHE INPUT_assimp PROPERTY STRINGS undefined qt system no)



#### Libraries



#### Tests



#### Features

qt_feature("qt3d_render" PUBLIC
    SECTION "Aspects"
    LABEL "Render aspect"
    PURPOSE "Use the 3D Render Aspect library"
)

qt_feature("qt3d_input" PUBLIC
    SECTION "Aspects"
    LABEL "Input aspect"
    PURPOSE "Use the 3D Input Aspect library"
)

qt_feature("qt3d_logic" PUBLIC
    SECTION "Aspects"
    LABEL "Logic aspect"
    PURPOSE "Use the 3D Logic Aspect library"
)

qt_feature("qt3d_extras" PUBLIC
    SECTION "Aspects"
    LABEL "Extras aspect"
    PURPOSE "Use the 3D Extra library"
    CONDITION QT_FEATURE_qt3d_render AND QT_FEATURE_qt3d_input AND QT_FEATURE_qt3d_logic
)

qt_feature("qt3d_animation" PUBLIC
    SECTION "Aspects"
    LABEL "Animation aspect"
    PURPOSE "Use the 3D Animation Aspect library"
    CONDITION QT_FEATURE_qt3d_render
)

qt_feature("qt3d_simd_sse2" PRIVATE
    LABEL "Use SSE2 instructions"
    PURPOSE "Use SSE2 SIMD instructions to accelerate matrix operations"
    AUTODETECT QT_FEATURE_sse2
    ENABLE INPUT_qt3d_simd STREQUAL 'sse2' OR INPUT_qt3d_simd STREQUAL 'avx2'
    DISABLE INPUT_qt3d_simd STREQUAL 'no' OR ( TEST_architecture_arch STREQUAL i386 )
)

qt_feature("qt3d_simd_avx2" PRIVATE
    LABEL "Use AVX2 instructions"
    PURPOSE "Use AVX2 SIMD instructions to accelerate matrix operations"
    AUTODETECT TEST_arch_${TEST_architecture_arch}_subarch_avx2
    ENABLE INPUT_qt3d_simd STREQUAL 'avx2'
    DISABLE INPUT_qt3d_simd STREQUAL 'sse2' OR INPUT_qt3d_simd STREQUAL 'no' OR ( TEST_architecture_arch STREQUAL i386 )
)

qt_configure_add_summary_section(NAME "Qt3D Core")
qt_configure_add_summary_entry(ARGS "qt3d_simd_sse2")
qt_configure_add_summary_entry(ARGS "qt3d_simd_avx2")
qt_configure_add_summary_entry(ARGS "qt3d_animation")
qt_configure_add_summary_entry(ARGS "qt3d_extras")
qt_configure_add_summary_entry(ARGS "qt3d_input")
qt_configure_add_summary_entry(ARGS "qt3d_logic")
qt_configure_add_summary_entry(ARGS "qt3d_render")
qt_configure_end_summary_section() # end of "Qt3D" section
