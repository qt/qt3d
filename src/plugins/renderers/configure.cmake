#### Inputs



#### Libraries



#### Tests



#### Features

qt_feature("qt3d_opengl_renderer" PRIVATE
    SECTION "Qt 3D Renderers"
    LABEL "OpenGL Renderer"
    PURPOSE "Build the OpenGL renderer"
)

qt_feature("qt3d_rhi_renderer" PRIVATE
    SECTION "Qt 3D Renderers"
    LABEL "RHI Renderer"
    PURPOSE "Build the RHI renderer"
)

qt_configure_add_summary_section(NAME "Qt3D Renderers")
qt_configure_add_summary_entry(ARGS "qt3d_opengl_renderer")
qt_configure_add_summary_entry(ARGS "qt3d_rhi_renderer")
qt_configure_end_summary_section() # end of "Qt3D" section
