INCLUDEPATH += $$PWD

include("framegraph/framegraph.pri")
include("jobs/render-jobs.pri")

HEADERS += \
    $$PWD/renderthread_p.h \
    $$PWD/renderconfiguration_p.h \
    $$PWD/renderer_p.h \
    $$PWD/quniformvalue_p.h \
    $$PWD/renderview_p.h \
    $$PWD/rendercommand_p.h \
    $$PWD/renderqueue_p.h \
    $$PWD/parameterpack_p.h \
    $$PWD/texturedatamanager_p.h \
    $$PWD/rendertarget_p.h \
    $$PWD/renderattachment_p.h \
    $$PWD/scenemanager_p.h \
    $$PWD/attachmentpack_p.h \
    $$PWD/shadervariables_p.h \
    $$PWD/qgraphicsutils_p.h \
    $$PWD/managers_p.h \
    $$PWD/handle_types_p.h \
    $$PWD/platformsurfacefilter_p.h \
    $$PWD/shaderdata_p.h \
    $$PWD/vsyncframeadvanceservice_p.h \
    $$PWD/qraycastingservice.h \
    $$PWD/qraycastingservice_p.h \
    $$PWD/annotation_p.h \
    $$PWD/cameralens_p.h \
    $$PWD/effect_p.h \
    $$PWD/entity_p.h \
    $$PWD/layer_p.h \
    $$PWD/material_p.h \
    $$PWD/nodefunctor_p.h \
    $$PWD/parameter_p.h \
    $$PWD/parametermapping_p.h \
    $$PWD/renderpass_p.h \
    $$PWD/scene_p.h \
    $$PWD/shaderdata_p.h \
    $$PWD/shader_p.h \
    $$PWD/technique_p.h \
    $$PWD/texture_p.h \
    $$PWD/textureimage_p.h \
    $$PWD/transform_p.h

SOURCES += \
    $$PWD/renderthread.cpp \
    $$PWD/renderconfiguration.cpp \
    $$PWD/renderer.cpp \
    $$PWD/quniformvalue.cpp \
    $$PWD/renderview.cpp \
    $$PWD/rendercommand.cpp \
    $$PWD/renderqueue.cpp \
    $$PWD/parameterpack.cpp \
    $$PWD/texturedatamanager.cpp \
    $$PWD/rendertarget.cpp \
    $$PWD/renderattachment.cpp \
    $$PWD/scenemanager.cpp \
    $$PWD/attachmentpack.cpp \
    $$PWD/platformsurfacefilter.cpp \
    $$PWD/qraycastingservice.cpp \
    $$PWD/vsyncframeadvanceservice.cpp \
    $$PWD/annotation.cpp \
    $$PWD/cameralens.cpp \
    $$PWD/effect.cpp \
    $$PWD/entity.cpp \
    $$PWD/layer.cpp \
    $$PWD/material.cpp \
    $$PWD/parameter.cpp \
    $$PWD/parametermapping.cpp \
    $$PWD/renderpass.cpp \
    $$PWD/scene.cpp \
    $$PWD/shaderdata.cpp \
    $$PWD/shader.cpp \
    $$PWD/technique.cpp \
    $$PWD/texture.cpp \
    $$PWD/textureimage.cpp \
    $$PWD/transform.cpp
