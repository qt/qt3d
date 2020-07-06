# Generated from rhi.pro.

#####################################################################
## RhiRendererPlugin Plugin:
#####################################################################

qt_internal_add_plugin(RhiRendererPlugin
    OUTPUT_NAME rhirenderer
    TYPE renderers
    SOURCES
        graphicshelpers/submissioncontext.cpp graphicshelpers/submissioncontext_p.h
        io/rhibuffer.cpp io/rhibuffer_p.h
        jobs/renderviewcommandbuilderjob.cpp jobs/renderviewcommandbuilderjob_p.h
        jobs/renderviewcommandupdaterjob.cpp jobs/renderviewcommandupdaterjob_p.h
        jobs/renderviewinitializerjob.cpp jobs/renderviewinitializerjob_p.h
        main.cpp
        managers/rhihandle_types_p.h
        managers/rhiresourcemanagers.cpp managers/rhiresourcemanagers_p.h
        renderer/commandexecuter.cpp renderer/commandexecuter_p.h
        renderer/logging.cpp renderer/logging_p.h
        renderer/rendercommand.cpp renderer/rendercommand_p.h
        renderer/renderer.cpp renderer/renderer_p.h
        renderer/renderercache_p.h
        renderer/renderview.cpp renderer/renderview_p.h
        renderer/renderviewbuilder.cpp renderer/renderviewbuilder_p.h
        renderer/rhigraphicspipeline.cpp renderer/rhigraphicspipeline_p.h
        renderer/rhirendertarget.cpp renderer/rhirendertarget_p.h
        renderer/rhishader.cpp renderer/rhishader_p.h
        renderer/shaderparameterpack.cpp renderer/shaderparameterpack_p.h
        renderer/shadervariables_p.h
        textures/renderbuffer.cpp textures/renderbuffer_p.h
        textures/texture.cpp textures/texture_p.h
    DEFINES
        QT_BUILDING_QT
        QT_NO_FOREACH QT_NO_FOREACH
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        graphicshelpers
        io
        jobs
        managers
        renderer
        textures
    PUBLIC_LIBRARIES
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

#### Keys ignored in scope 1:.:.:rhi.pro:<TRUE>:
# DISTFILES = "rhirenderer.json"

## Scopes:
#####################################################################

qt_extend_target(RhiRendererPlugin CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
