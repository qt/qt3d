# Generated from benchmarks.pro.

if(NOT QT_FEATURE_qt3d_extras)
    return()
endif()

add_subdirectory(core)
if(QT_FEATURE_qt3d_render)
# Disabled temporarily as some benchmarks fail to build on Mac
#    add_subdirectory(render)
endif()
