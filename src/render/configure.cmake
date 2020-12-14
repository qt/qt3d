

#### Inputs



#### Libraries



#### Tests



#### Features

qt_feature("qt3d-opengl-renderer" PRIVATE
    SECTION "Qt 3D Renderers"
    LABEL "OpenGL Renderer"
    PURPOSE "Use the OpenGL renderer"
)
qt_feature("qt3d-rhi-renderer" PRIVATE
    SECTION "Qt 3D Renderers"
    LABEL "RHI Renderer"
    PURPOSE "Use the RHI renderer"
    AUTODETECT OFF
)
qt_configure_add_summary_section(NAME "Qt 3D Renderers")
qt_configure_add_summary_entry(ARGS "qt3d-opengl-renderer")
qt_configure_add_summary_entry(ARGS "qt3d-rhi-renderer")
qt_configure_end_summary_section() # end of "Qt 3D Renderers" section
