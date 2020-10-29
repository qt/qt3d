

#### Inputs

# input assimp
set(INPUT_assimp "undefined" CACHE STRING "")
set_property(CACHE INPUT_assimp PROPERTY STRINGS undefined no qt system)


#### Libraries

qt_find_package(WrapAssimp 5 PROVIDED_TARGETS WrapAssimp::WrapAssimp)

qt_config_compile_test("qt3d_assimp"
                   LABEL "Assimp"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../config.tests/assimp"
                   LIBRARIES WrapAssimp::WrapAssimp
                   PACKAGES PACKAGE WrapAssimp 5)

#### Tests



#### Features

qt_feature("qt3d_assimp" PUBLIC PRIVATE
    LABEL "Assimp"
)
qt_feature_definition("qt3d_assimp" "QT_NO_ASSIMP" NEGATE VALUE "1")
qt_feature("qt3d_system_assimp" PRIVATE
    LABEL "System Assimp"
    CONDITION QT_FEATURE_qt3d_assimp AND TEST_assimp
    ENABLE INPUT_assimp STREQUAL 'system'
    DISABLE INPUT_assimp STREQUAL 'qt'
)
qt_configure_add_summary_section(NAME "Qt3D Scene Parsers")
qt_configure_add_summary_entry(ARGS "qt3d_assimp")
qt_configure_add_summary_entry(ARGS "qt3d_system_assimp")
qt_configure_end_summary_section() # end of "Qt3D" section
