# Generated from renderers.pro.

if(QT_FEATURE_qt3d_opengl_renderer)
    add_subdirectory(opengl)
endif()
if(QT_FEATURE_qt3d_rhi_renderer AND TARGET Qt::ShaderTools)
    add_subdirectory(rhi)
endif()
