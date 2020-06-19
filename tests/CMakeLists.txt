# Generated from tests.pro.

if(QT_BUILD_STANDALONE_TESTS)
    # Add qt_find_package calls for extra dependencies that need to be found when building
    # the standalone tests here.
endif()

# many tests rely on extras to load data
if(QT_FEATURE_qt3d_extras)
    qt_build_tests()

    add_subdirectory(manual)  # add here since default seems to be to not build them
    if(NOT QT_FEATURE_testcocoon AND CMAKE_BUILD_TYPE STREQUAL Release)
        add_subdirectory(benchmarks)
    endif()
endif()
