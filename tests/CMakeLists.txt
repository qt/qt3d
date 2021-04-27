# Generated from tests.pro.

if(QT_BUILD_STANDALONE_TESTS)
    # Add qt_find_package calls for extra dependencies that need to be found when building
    # the standalone tests here.
endif()

# many tests rely on extras to load data
if(QT_FEATURE_qt3d_extras)
    qt_build_tests()
endif()
