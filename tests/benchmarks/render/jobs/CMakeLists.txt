# Generated from jobs.pro.

#####################################################################
## tst_bench_jobs Binary:
#####################################################################

qt_add_benchmark(tst_bench_jobs
    SOURCES
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicscontext.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicscontext_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes2.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes2_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3_1.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3_1_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3_2.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperes3_2_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl2.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl2_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl3_2.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl3_2_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl3_3.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl3_3_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl4.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelpergl4_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/graphicshelperinterface_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/imagesubmissioncontext.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/imagesubmissioncontext_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/qgraphicsutils_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/submissioncontext.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/submissioncontext_p.h
        ../../../../src/plugins/renderers/opengl/graphicshelpers/texturesubmissioncontext.cpp ../../../../src/plugins/renderers/opengl/graphicshelpers/texturesubmissioncontext_p.h
        ../../../../src/plugins/renderers/opengl/io/glbuffer.cpp ../../../../src/plugins/renderers/opengl/io/glbuffer_p.h
        ../../../../src/plugins/renderers/opengl/jobs/filtercompatibletechniquejob.cpp ../../../../src/plugins/renderers/opengl/jobs/filtercompatibletechniquejob_p.h
        ../../../../src/plugins/renderers/opengl/jobs/materialparametergathererjob.cpp ../../../../src/plugins/renderers/opengl/jobs/materialparametergathererjob_p.h
        ../../../../src/plugins/renderers/opengl/jobs/renderviewcommandbuilderjob.cpp ../../../../src/plugins/renderers/opengl/jobs/renderviewcommandbuilderjob_p.h
        ../../../../src/plugins/renderers/opengl/jobs/renderviewcommandupdaterjob.cpp ../../../../src/plugins/renderers/opengl/jobs/renderviewcommandupdaterjob_p.h
        ../../../../src/plugins/renderers/opengl/jobs/renderviewinitializerjob.cpp ../../../../src/plugins/renderers/opengl/jobs/renderviewinitializerjob_p.h
        ../../../../src/plugins/renderers/opengl/jobs/renderviewjobutils.cpp ../../../../src/plugins/renderers/opengl/jobs/renderviewjobutils_p.h
        ../../../../src/plugins/renderers/opengl/managers/gl_handle_types_p.h
        ../../../../src/plugins/renderers/opengl/managers/glresourcemanagers.cpp ../../../../src/plugins/renderers/opengl/managers/glresourcemanagers_p.h
        ../../../../src/plugins/renderers/opengl/renderer/commandexecuter.cpp ../../../../src/plugins/renderers/opengl/renderer/commandexecuter_p.h
        ../../../../src/plugins/renderers/opengl/renderer/frameprofiler_p.h
        ../../../../src/plugins/renderers/opengl/renderer/glfence_p.h
        ../../../../src/plugins/renderers/opengl/renderer/gllights.cpp ../../../../src/plugins/renderers/opengl/renderer/gllights_p.h
        ../../../../src/plugins/renderers/opengl/renderer/glshader.cpp ../../../../src/plugins/renderers/opengl/renderer/glshader_p.h
        ../../../../src/plugins/renderers/opengl/renderer/logging.cpp ../../../../src/plugins/renderers/opengl/renderer/logging_p.h
        ../../../../src/plugins/renderers/opengl/renderer/openglvertexarrayobject.cpp ../../../../src/plugins/renderers/opengl/renderer/openglvertexarrayobject_p.h
        ../../../../src/plugins/renderers/opengl/renderer/rendercommand.cpp ../../../../src/plugins/renderers/opengl/renderer/rendercommand_p.h
        ../../../../src/plugins/renderers/opengl/renderer/renderer.cpp ../../../../src/plugins/renderers/opengl/renderer/renderer_p.h
        ../../../../src/plugins/renderers/opengl/renderer/renderercache_p.h
        ../../../../src/plugins/renderers/opengl/renderer/renderqueue.cpp ../../../../src/plugins/renderers/opengl/renderer/renderqueue_p.h
        ../../../../src/plugins/renderers/opengl/renderer/renderview.cpp ../../../../src/plugins/renderers/opengl/renderer/renderview_p.h
        ../../../../src/plugins/renderers/opengl/renderer/renderviewbuilder.cpp ../../../../src/plugins/renderers/opengl/renderer/renderviewbuilder_p.h
        ../../../../src/plugins/renderers/opengl/renderer/shaderparameterpack.cpp ../../../../src/plugins/renderers/opengl/renderer/shaderparameterpack_p.h
        ../../../../src/plugins/renderers/opengl/renderer/shadervariables_p.h
        ../../../../src/plugins/renderers/opengl/textures/gltexture.cpp ../../../../src/plugins/renderers/opengl/textures/gltexture_p.h
        ../../../../src/plugins/renderers/opengl/textures/renderbuffer.cpp ../../../../src/plugins/renderers/opengl/textures/renderbuffer_p.h
        tst_bench_jobs.cpp
    DEFINES
        BUILD_QT3D_MODULE
        QT3D_RENDER_UNIT_TESTS
        QT_BUILDING_QT
        QT_BUILD_INTERNAL
        QT_NO_FOREACH QT_NO_FOREACH
    INCLUDE_DIRECTORIES
        ../../../../src/plugins/renderers/opengl
        ../../../../src/plugins/renderers/opengl/graphicshelpers
        ../../../../src/plugins/renderers/opengl/io
        ../../../../src/plugins/renderers/opengl/jobs
        ../../../../src/plugins/renderers/opengl/managers
        ../../../../src/plugins/renderers/opengl/renderer
        ../../../../src/plugins/renderers/opengl/textures
    LIBRARIES
        Qt::OpenGLExtensions
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DQuick
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
        Qt::OpenGLPrivate
        Qt::Qml
        Qt::Test
)

#### Keys ignored in scope 1:.:.:jobs.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_bench_jobs CONDITION NOT INTEGRITY
    SOURCES
        ../../../../src/plugins/renderers/opengl/debug/imguirenderer.cpp ../../../../src/plugins/renderers/opengl/debug/imguirenderer_p.h
    INCLUDE_DIRECTORIES
        ../../../../src/plugins/renderers/opengl/debug
)

qt_extend_target(tst_bench_jobs CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
