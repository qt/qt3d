# Generated from tests.pro.

if(QT_BUILD_STANDALONE_TESTS)
    # Add qt_find_package calls for extra dependencies that need to be found when building
    # the standalone tests here.
endif()

qt_build_tests()

add_subdirectory(manual)  # add here since default seems to be to not build them
