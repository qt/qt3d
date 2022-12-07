# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause



#### Inputs



#### Libraries

if(NOT QT_CONFIGURE_RUNNING)
    find_package(Vulkan)
endif()

#### Tests



#### Features

qt_feature("qt3d-opengl-renderer" PUBLIC
    SECTION "Qt 3D Renderers"
    LABEL "OpenGL Renderer"
    PURPOSE "Use the OpenGL renderer"
)
qt_feature("qt3d-rhi-renderer" PUBLIC
    SECTION "Qt 3D Renderers"
    LABEL "RHI Renderer"
    PURPOSE "Use the RHI renderer (requires qtShaderTools module)"
    CONDITION TARGET Qt::ShaderTools
)
qt_feature("qt3d-vulkan" PUBLIC
    LABEL "Vulkan"
    CONDITION Vulkan_FOUND AND QT_FEATURE_qt3d_rhi_renderer AND QT_FEATURE_vulkan
)
qt_configure_add_summary_section(NAME "Qt 3D APIs")
qt_configure_add_summary_entry(ARGS "qt3d-vulkan")
qt_configure_end_summary_section() # end of "Qt 3D APIs" section
qt_configure_add_summary_section(NAME "Qt 3D Renderers")
qt_configure_add_summary_entry(ARGS "qt3d-opengl-renderer")
qt_configure_add_summary_entry(ARGS "qt3d-rhi-renderer")
qt_configure_end_summary_section() # end of "Qt 3D Renderers" section
