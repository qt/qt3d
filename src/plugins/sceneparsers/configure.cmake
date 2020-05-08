

#### Inputs

# input assimp
set(INPUT_assimp "undefined" CACHE STRING "")
set_property(CACHE INPUT_assimp PROPERTY STRINGS undefined no qt system)


#### Libraries

qt_find_package(WrapAssimp 5 PROVIDED_TARGETS WrapAssimp::WrapAssimp)

qt_config_compile_test("assimp"
                   LABEL "Assimp"
                   PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../config.tests/assimp"
                   LIBRARIES WrapAssimp::WrapAssimp
                   PACKAGES PACKAGE WrapAssimp 5)

#### Tests



#### Features

qt_feature("assimp" PUBLIC PRIVATE
    LABEL "Assimp"
)
qt_feature_definition("assimp" "QT_NO_ASSIMP" NEGATE VALUE "1")
qt_feature("system-assimp" PRIVATE
    LABEL "System Assimp"
    CONDITION QT_FEATURE_assimp AND TEST_assimp
    ENABLE INPUT_assimp STREQUAL 'system'
    DISABLE INPUT_assimp STREQUAL 'qt'
)
qt_configure_add_summary_section(NAME "Qt3D Scene Parsers")
qt_configure_add_summary_entry(ARGS "assimp")
qt_configure_add_summary_entry(ARGS "system-assimp")
qt_configure_end_summary_section() # end of "Qt3D" section
