# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rhi.pro.

set(rhiRendererSources graphicshelpers/submissioncontext.cpp graphicshelpers/submissioncontext_p.h
    io/rhibuffer.cpp io/rhibuffer_p.h
    managers/rhihandle_types_p.h
    managers/rhiresourcemanagers.cpp managers/rhiresourcemanagers_p.h
    renderer/commandexecuter.cpp renderer/commandexecuter_p.h
    renderer/logging.cpp renderer/logging_p.h
    renderer/rendercommand.cpp renderer/rendercommand_p.h
    renderer/renderer.cpp renderer/renderer_p.h
    renderer/renderview.cpp renderer/renderview_p.h
    renderer/renderviewbuilder.cpp renderer/renderviewbuilder_p.h
    renderer/rhigraphicspipeline.cpp renderer/rhigraphicspipeline_p.h
    renderer/rhirendertarget.cpp renderer/rhirendertarget_p.h
    renderer/rhishader.cpp renderer/rhishader_p.h
    renderer/shaderparameterpack.cpp renderer/shaderparameterpack_p.h
    renderer/shadervariables_p.h
    renderer/pipelineuboset.cpp renderer/pipelineuboset_p.h
    textures/renderbuffer.cpp textures/renderbuffer_p.h
    textures/texture.cpp textures/texture_p.h
)

#####################################################################
## Static library used for Unit Tests:
#####################################################################

if(QT_FEATURE_private_tests)

    add_library(RhiRendererLib STATIC ${rhiRendererSources})

    target_include_directories(RhiRendererLib PUBLIC
        managers
        io
        graphicshelpers
        renderer
        textures
    )

    target_link_libraries(RhiRendererLib
        PUBLIC
            Qt::3DCore
            Qt::3DCorePrivate
            Qt::3DRender
            Qt::3DRenderPrivate
            Qt::Core
            Qt::CorePrivate
            Qt::Gui
            Qt::GuiPrivate
            Qt::ShaderTools
            Qt::ShaderToolsPrivate
    )

    target_compile_definitions(RhiRendererLib PRIVATE
        BUILD_QT3D_MODULE
        QT_BUILDING_QT
    )

endif()

#####################################################################
## RhiRendererPlugin Plugin:
#####################################################################

qt_internal_add_plugin(RhiRendererPlugin
    OUTPUT_NAME rhirenderer
    PLUGIN_TYPE renderers
    SOURCES
        main.cpp
        ${rhiRendererSources}
    DEFINES
        BUILD_QT3D_MODULE
        QT_BUILDING_QT
        QT_NO_FOREACH QT_NO_FOREACH
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        graphicshelpers
        io
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
        Qt::ShaderTools
        Qt::ShaderToolsPrivate
)

# QTransform conflicts
set_target_properties(RhiRendererPlugin PROPERTIES UNITY_BUILD OFF)

#### Keys ignored in scope 1:.:.:rhi.pro:<TRUE>:
# DISTFILES = "rhirenderer.json"

## Scopes:
#####################################################################

qt_internal_extend_target(RhiRendererPlugin CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
