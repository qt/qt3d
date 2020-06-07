# Generated from examples.pro.

#special case begin
# Examples are disable for now on MSVC - big_resources does not seem to
# have been translated into CMake automtically yet, and trying to replace it
# manually with qt6_add_big_resources fails as of june 2020.
# Disable that temporarily to be able to test on Win32
if(MSVC)
    return()
endif()

#special case end
qt_examples_build_begin()

if(QT_FEATURE_qt3d_extras)
    add_subdirectory(qt3d)
endif()

qt_examples_build_end()
