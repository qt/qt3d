# Generated from benchmarks.pro.

add_subdirectory(core)
if(QT_FEATURE_qt3d_render)
    add_subdirectory(render)
endif()
