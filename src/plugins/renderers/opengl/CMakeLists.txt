# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from opengl.pro.

set(openGLRendererSources graphicshelpers/graphicscontext.cpp graphicshelpers/graphicscontext_p.h
    graphicshelpers/graphicshelperes2.cpp graphicshelpers/graphicshelperes2_p.h
    graphicshelpers/graphicshelperes3.cpp graphicshelpers/graphicshelperes3_p.h
    graphicshelpers/graphicshelperes3_1.cpp graphicshelpers/graphicshelperes3_1_p.h
    graphicshelpers/graphicshelperes3_2.cpp graphicshelpers/graphicshelperes3_2_p.h
    graphicshelpers/graphicshelpergl2.cpp graphicshelpers/graphicshelpergl2_p.h
    graphicshelpers/graphicshelpergl3_2.cpp graphicshelpers/graphicshelpergl3_2_p.h
    graphicshelpers/graphicshelpergl3_3.cpp graphicshelpers/graphicshelpergl3_3_p.h
    graphicshelpers/graphicshelpergl4.cpp graphicshelpers/graphicshelpergl4_p.h
    graphicshelpers/graphicshelperinterface_p.h
    graphicshelpers/imagesubmissioncontext.cpp graphicshelpers/imagesubmissioncontext_p.h
    graphicshelpers/qgraphicsutils_p.h
    graphicshelpers/submissioncontext.cpp graphicshelpers/submissioncontext_p.h
    graphicshelpers/texturesubmissioncontext.cpp graphicshelpers/texturesubmissioncontext_p.h
    io/glbuffer.cpp io/glbuffer_p.h
    managers/gl_handle_types_p.h
    managers/glresourcemanagers.cpp managers/glresourcemanagers_p.h
    renderer/commandexecuter.cpp renderer/commandexecuter_p.h
    renderer/frameprofiler_p.h
    renderer/glfence_p.h
    renderer/gllights.cpp renderer/gllights_p.h
    renderer/glshader.cpp renderer/glshader_p.h
    renderer/logging.cpp renderer/logging_p.h
    renderer/openglvertexarrayobject.cpp renderer/openglvertexarrayobject_p.h
    renderer/rendercommand.cpp renderer/rendercommand_p.h
    renderer/renderer.cpp renderer/renderer_p.h
    renderer/renderview.cpp renderer/renderview_p.h
    renderer/renderviewbuilder.cpp renderer/renderviewbuilder_p.h
    renderer/shaderparameterpack.cpp renderer/shaderparameterpack_p.h
    renderer/shadervariables_p.h
    textures/gltexture.cpp textures/gltexture_p.h
    textures/renderbuffer.cpp textures/renderbuffer_p.h
)

#####################################################################
## Static library used for Unit Tests:
#####################################################################

if(QT_FEATURE_private_tests)

    add_library(OpenGLRendererLib STATIC ${openGLRendererSources})

    target_include_directories(OpenGLRendererLib PUBLIC
        graphicshelpers
        io
        jobs
        managers
        renderer
        textures
    )

    target_link_libraries(OpenGLRendererLib
        PUBLIC
            Qt::3DCore
            Qt::3DCorePrivate
            Qt::3DRender
            Qt::3DRenderPrivate
            Qt::Core
            Qt::CorePrivate
            Qt::Gui
            Qt::GuiPrivate
            Qt::OpenGL
            Qt::OpenGLPrivate
    )

    target_compile_definitions(OpenGLRendererLib PRIVATE
        BUILD_QT3D_MODULE
        QT_BUILDING_QT
        QT_NO_FOREACH
    )

    set(openGLRendererImguiSources
            debug/imguirenderer.cpp debug/imguirenderer_p.h
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui.cpp
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_draw.cpp
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_tables.cpp
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_widgets.cpp
    )

    qt_internal_extend_sbom(OpenGLRendererLib TYPE CUSTOM)
    qt_internal_extend_target(OpenGLRendererLib CONDITION NOT INTEGRITY
        SOURCES
            ${openGLRendererImguiSources}
        NO_PCH_SOURCES
            ${openGLRendererImguiSources} # various defines (e.g. IMGUI_DEFINE_MATH_OPERATORS)
        INCLUDE_DIRECTORIES
            debug
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui
        ATTRIBUTION_FILE_DIR_PATHS
            ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui
    )

endif()

#####################################################################
## OpenGLRendererPlugin Plugin:
#####################################################################

qt_internal_add_plugin(OpenGLRendererPlugin
    OUTPUT_NAME openglrenderer
    PLUGIN_TYPE renderers
    SOURCES
        main.cpp
        ${openGLRendererSources}
    DEFINES
        BUILD_QT3D_MODULE
        QT_BUILDING_QT
        QT_NO_FOREACH
    INCLUDE_DIRECTORIES
        graphicshelpers
        io
        jobs
        managers
        renderer
        textures
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
        Qt::OpenGL
        Qt::OpenGLPrivate
)

# QTransform conflicts
set_target_properties(OpenGLRendererPlugin PROPERTIES UNITY_BUILD OFF)

qt_internal_extend_target(OpenGLRendererPlugin CONDITION NOT INTEGRITY
    SOURCES
        debug/imguirenderer.cpp debug/imguirenderer_p.h
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui.cpp
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_tables.cpp
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui/imgui_widgets.cpp
    INCLUDE_DIRECTORIES
        debug
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui
    ATTRIBUTION_FILE_DIR_PATHS
        ${PROJECT_SOURCE_DIR}/src/3rdparty/imgui
)

## Scopes:
#####################################################################

qt_internal_extend_target(OpenGLRendererPlugin CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
