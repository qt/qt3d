# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from render.pro.

#####################################################################
## 3drender Module:
#####################################################################

qt_internal_add_module(3DRender
    PLUGIN_TYPES sceneparsers geometryloaders renderplugins renderers
    SOURCES
        backend/abstractrenderer_p.h
        backend/apishadermanager_p.h
        backend/attachmentpack.cpp backend/attachmentpack_p.h
        backend/backendnode.cpp backend/backendnode_p.h
        backend/boundingvolumedebug.cpp backend/boundingvolumedebug_p.h
        backend/bufferutils_p.h
        backend/buffervisitor_p.h
        backend/cameralens.cpp backend/cameralens_p.h
        backend/computecommand.cpp backend/computecommand_p.h
        backend/entity.cpp backend/entity_p.h
        backend/entity_p_p.h
        backend/entityaccumulator.cpp backend/entityaccumulator_p.h
        backend/entityvisitor.cpp backend/entityvisitor_p.h
        backend/handle_types_p.h
        backend/layer.cpp backend/layer_p.h
        backend/levelofdetail.cpp backend/levelofdetail_p.h
        backend/managers.cpp backend/managers_p.h
        backend/nodefunctor_p.h
        backend/nodemanagers.cpp backend/nodemanagers_p.h
        backend/offscreensurfacehelper.cpp backend/offscreensurfacehelper_p.h
        backend/parameterpack.cpp backend/parameterpack_p.h
        backend/platformsurfacefilter.cpp backend/platformsurfacefilter_p.h
        backend/pointsvisitor.cpp backend/pointsvisitor_p.h
        backend/rendersettings.cpp backend/rendersettings_p.h
        backend/rendertarget.cpp backend/rendertarget_p.h
        backend/rendertargetoutput.cpp backend/rendertargetoutput_p.h
        backend/resourceaccessor.cpp backend/resourceaccessor_p.h
        backend/segmentsvisitor.cpp backend/segmentsvisitor_p.h
        backend/stringtoint.cpp backend/stringtoint_p.h
        backend/transform.cpp backend/transform_p.h
        backend/triangleboundingvolume.cpp backend/triangleboundingvolume_p.h
        backend/trianglesvisitor.cpp backend/trianglesvisitor_p.h
        backend/uniform.cpp backend/uniform_p.h
        backend/visitorutils_p.h
        framegraph/blitframebuffer.cpp framegraph/blitframebuffer_p.h
        framegraph/buffercapture.cpp framegraph/buffercapture_p.h
        framegraph/cameraselectornode.cpp framegraph/cameraselectornode_p.h
        framegraph/clearbuffers.cpp framegraph/clearbuffers_p.h
        framegraph/debugoverlay.cpp framegraph/debugoverlay_p.h
        framegraph/dispatchcompute.cpp framegraph/dispatchcompute_p.h
        framegraph/framegraphnode.cpp framegraph/framegraphnode_p.h
        framegraph/framegraphvisitor.cpp framegraph/framegraphvisitor_p.h
        framegraph/frustumculling.cpp framegraph/frustumculling_p.h
        framegraph/layerfilternode.cpp framegraph/layerfilternode_p.h
        framegraph/memorybarrier.cpp framegraph/memorybarrier_p.h
        framegraph/nodraw.cpp framegraph/nodraw_p.h
        framegraph/nopicking.cpp framegraph/nopicking_p.h
        framegraph/proximityfilter.cpp framegraph/proximityfilter_p.h
        framegraph/qblitframebuffer.cpp framegraph/qblitframebuffer.h framegraph/qblitframebuffer_p.h
        framegraph/qbuffercapture.cpp framegraph/qbuffercapture.h framegraph/qbuffercapture_p.h
        framegraph/qcameraselector.cpp framegraph/qcameraselector.h framegraph/qcameraselector_p.h
        framegraph/qclearbuffers.cpp framegraph/qclearbuffers.h framegraph/qclearbuffers_p.h
        framegraph/qdebugoverlay.cpp framegraph/qdebugoverlay.h framegraph/qdebugoverlay_p.h
        framegraph/qdispatchcompute.cpp framegraph/qdispatchcompute.h framegraph/qdispatchcompute_p.h
        framegraph/qframegraphnode.cpp framegraph/qframegraphnode.h framegraph/qframegraphnode_p.h
        framegraph/qfrustumculling.cpp framegraph/qfrustumculling.h
        framegraph/qlayerfilter.cpp framegraph/qlayerfilter.h framegraph/qlayerfilter_p.h
        framegraph/qmemorybarrier.cpp framegraph/qmemorybarrier.h framegraph/qmemorybarrier_p.h
        framegraph/qnodraw.cpp framegraph/qnodraw.h
        framegraph/qnopicking.cpp framegraph/qnopicking.h
        framegraph/qproximityfilter.cpp framegraph/qproximityfilter.h framegraph/qproximityfilter_p.h
        framegraph/qrendercapture.cpp framegraph/qrendercapture.h framegraph/qrendercapture_p.h
        framegraph/qrenderpassfilter.cpp framegraph/qrenderpassfilter.h framegraph/qrenderpassfilter_p.h
        framegraph/qrenderstateset.cpp framegraph/qrenderstateset.h framegraph/qrenderstateset_p.h
        framegraph/qrendersurfaceselector.cpp framegraph/qrendersurfaceselector.h framegraph/qrendersurfaceselector_p.h
        framegraph/qrendertargetselector.cpp framegraph/qrendertargetselector.h framegraph/qrendertargetselector_p.h
        framegraph/qsetfence.cpp framegraph/qsetfence.h framegraph/qsetfence_p.h
        framegraph/qsortpolicy.cpp framegraph/qsortpolicy.h framegraph/qsortpolicy_p.h
        framegraph/qsubtreeenabler.cpp framegraph/qsubtreeenabler.h framegraph/qsubtreeenabler_p.h
        framegraph/qtechniquefilter.cpp framegraph/qtechniquefilter.h framegraph/qtechniquefilter_p.h
        framegraph/qviewport.cpp framegraph/qviewport.h framegraph/qviewport_p.h
        framegraph/qwaitfence.cpp framegraph/qwaitfence.h framegraph/qwaitfence_p.h
        framegraph/rendercapture.cpp framegraph/rendercapture_p.h
        framegraph/renderpassfilternode.cpp framegraph/renderpassfilternode_p.h
        framegraph/rendersurfaceselector.cpp framegraph/rendersurfaceselector_p.h
        framegraph/rendertargetselectornode.cpp framegraph/rendertargetselectornode_p.h
        framegraph/setfence.cpp framegraph/setfence_p.h
        framegraph/sortpolicy.cpp framegraph/sortpolicy_p.h
        framegraph/statesetnode.cpp framegraph/statesetnode_p.h
        framegraph/subtreeenabler.cpp framegraph/subtreeenabler_p.h
        framegraph/techniquefilternode.cpp framegraph/techniquefilternode_p.h
        framegraph/viewportnode.cpp framegraph/viewportnode_p.h
        framegraph/waitfence.cpp framegraph/waitfence_p.h
        frontend/qcamera.cpp frontend/qcamera.h frontend/qcamera_p.h
        frontend/qcameralens.cpp frontend/qcameralens.h frontend/qcameralens_p.h
        frontend/qcomputecommand.cpp frontend/qcomputecommand.h frontend/qcomputecommand_p.h
        frontend/qlayer.cpp frontend/qlayer.h frontend/qlayer_p.h
        frontend/qlevelofdetail.cpp frontend/qlevelofdetail.h frontend/qlevelofdetail_p.h
        frontend/qlevelofdetailboundingsphere.cpp frontend/qlevelofdetailboundingsphere.h
        frontend/qlevelofdetailswitch.cpp frontend/qlevelofdetailswitch.h frontend/qlevelofdetailswitch_p.h
        frontend/qpickingsettings.cpp frontend/qpickingsettings.h frontend/qpickingsettings_p.h
        frontend/qrenderapi.h
        frontend/qrenderaspect.cpp frontend/qrenderaspect.h frontend/qrenderaspect_p.h
        frontend/qrendercapabilities.cpp frontend/qrendercapabilities.h frontend/qrendercapabilities_p.h
        frontend/qrenderplugin_p.h
        frontend/qrenderpluginfactory.cpp frontend/qrenderpluginfactory_p.h
        frontend/qrenderpluginfactoryif.cpp frontend/qrenderpluginfactoryif_p.h
        frontend/qrendersettings.cpp frontend/qrendersettings.h frontend/qrendersettings_p.h
        frontend/qrendertarget.cpp frontend/qrendertarget.h frontend/qrendertarget_p.h
        frontend/qrendertargetoutput.cpp frontend/qrendertargetoutput.h frontend/qrendertargetoutput_p.h
        frontend/sphere.cpp frontend/sphere_p.h
        geometry/armature.cpp geometry/armature_p.h
        geometry/attribute.cpp geometry/attribute_p.h
        geometry/buffer.cpp geometry/buffer_p.h
        geometry/buffermanager.cpp geometry/buffermanager_p.h
        geometry/geometry.cpp geometry/geometry_p.h
        geometry/geometryrenderer.cpp geometry/geometryrenderer_p.h
        geometry/geometryrenderermanager.cpp geometry/geometryrenderermanager_p.h
        geometry/gltfskeletonloader.cpp geometry/gltfskeletonloader_p.h
        geometry/joint.cpp geometry/joint_p.h
        geometry/pickingproxy.cpp geometry/pickingproxy_p.h
        geometry/qgeometryrenderer.cpp geometry/qgeometryrenderer.h geometry/qgeometryrenderer_p.h
        geometry/qmesh.cpp geometry/qmesh.h geometry/qmesh_p.h
        geometry/qpickingproxy.cpp geometry/qpickingproxy.h geometry/qpickingproxy_p.h
        geometry/skeleton.cpp geometry/skeleton_p.h
        geometry/skeletondata.cpp geometry/skeletondata_p.h
        io/qaxisalignedboundingbox.cpp io/qaxisalignedboundingbox_p.h
        io/qgeometryloaderfactory.cpp io/qgeometryloaderfactory_p.h
        io/qgeometryloaderinterface_p.h
        io/qsceneexporter.cpp io/qsceneexporter_p.h
        io/qsceneexportfactory.cpp io/qsceneexportfactory_p.h
        io/qsceneexportplugin.cpp io/qsceneexportplugin_p.h
        io/qsceneimporter.cpp io/qsceneimporter_p.h
        io/qsceneimportfactory.cpp io/qsceneimportfactory_p.h
        io/qsceneimportplugin.cpp io/qsceneimportplugin_p.h
        io/qsceneloader.cpp io/qsceneloader.h io/qsceneloader_p.h
        io/scene.cpp io/scene_p.h
        io/scenemanager.cpp io/scenemanager_p.h
        jobs/abstractpickingjob.cpp jobs/abstractpickingjob_p.h
        jobs/calcboundingvolumejob.cpp jobs/calcboundingvolumejob_p.h
        jobs/computefilteredboundingvolumejob.cpp jobs/computefilteredboundingvolumejob_p.h
        jobs/expandboundingvolumejob.cpp jobs/expandboundingvolumejob_p.h
        jobs/filterentitybycomponentjob_p.h
        jobs/filterlayerentityjob.cpp jobs/filterlayerentityjob_p.h
        jobs/filterproximitydistancejob.cpp jobs/filterproximitydistancejob_p.h
        jobs/framecleanupjob.cpp jobs/framecleanupjob_p.h
        jobs/frustumcullingjob.cpp jobs/frustumcullingjob_p.h
        jobs/genericlambdajob_p.h
        jobs/job_common_p.h
        jobs/lightgatherer.cpp jobs/lightgatherer_p.h
        jobs/loadgeometryjob.cpp jobs/loadgeometryjob_p.h
        jobs/loadscenejob.cpp jobs/loadscenejob_p.h
        jobs/loadskeletonjob.cpp jobs/loadskeletonjob_p.h
        jobs/pickboundingvolumejob.cpp jobs/pickboundingvolumejob_p.h
        jobs/pickboundingvolumeutils.cpp jobs/pickboundingvolumeutils_p.h
        jobs/raycastingjob.cpp jobs/raycastingjob_p.h
        jobs/sendbuffercapturejob.cpp jobs/sendbuffercapturejob_p.h
        jobs/updateentitylayersjob.cpp jobs/updateentitylayersjob_p.h
        jobs/updatelevelofdetailjob.cpp jobs/updatelevelofdetailjob_p.h
        jobs/updateshaderdatatransformjob.cpp jobs/updateshaderdatatransformjob_p.h
        jobs/updateskinningpalettejob.cpp jobs/updateskinningpalettejob_p.h
        jobs/updatetreeenabledjob.cpp jobs/updatetreeenabledjob_p.h
        jobs/updateworldboundingvolumejob.cpp jobs/updateworldboundingvolumejob_p.h
        jobs/updateworldtransformjob.cpp jobs/updateworldtransformjob_p.h
        jobs/filtercompatibletechniquejob.cpp jobs/filtercompatibletechniquejob_p.h
        jobs/materialparametergathererjob.cpp jobs/materialparametergathererjob_p.h
        jobs/renderviewjobutils.cpp jobs/renderviewjobutils_p.h
        jobs/uniformblockbuilder.cpp jobs/uniformblockbuilder_p.h
        jobs/renderqueue_p.h
        jobs/renderercache_p.h
        jobs/renderviewcommandbuilderjob_p.h
        jobs/renderviewcommandupdaterjob_p.h
        jobs/renderviewinitializerjob_p.h
        jobs/rendersyncjobs_p.h
        lights/environmentlight.cpp lights/environmentlight_p.h
        lights/light.cpp lights/light_p.h
        lights/lightsource.cpp lights/lightsource_p.h
        lights/qabstractlight.cpp lights/qabstractlight.h lights/qabstractlight_p.h
        lights/qdirectionallight.cpp lights/qdirectionallight.h lights/qdirectionallight_p.h
        lights/qenvironmentlight.cpp lights/qenvironmentlight.h lights/qenvironmentlight_p.h
        lights/qpointlight.cpp lights/qpointlight.h lights/qpointlight_p.h
        lights/qspotlight.cpp lights/qspotlight.h lights/qspotlight_p.h
        materialsystem/effect.cpp materialsystem/effect_p.h
        materialsystem/filterkey.cpp materialsystem/filterkey_p.h
        materialsystem/material.cpp materialsystem/material_p.h
        materialsystem/parameter.cpp materialsystem/parameter_p.h
        materialsystem/qeffect.cpp materialsystem/qeffect.h materialsystem/qeffect_p.h
        materialsystem/qfilterkey.cpp materialsystem/qfilterkey.h materialsystem/qfilterkey_p.h
        materialsystem/qgraphicsapifilter.cpp materialsystem/qgraphicsapifilter.h materialsystem/qgraphicsapifilter_p.h
        materialsystem/qmaterial.cpp materialsystem/qmaterial.h materialsystem/qmaterial_p.h
        materialsystem/qparameter.cpp materialsystem/qparameter.h materialsystem/qparameter_p.h
        materialsystem/qrenderpass.cpp materialsystem/qrenderpass.h materialsystem/qrenderpass_p.h
        materialsystem/qshaderdata.cpp materialsystem/qshaderdata.h materialsystem/qshaderdata_p.h
        materialsystem/qshaderimage.cpp materialsystem/qshaderimage.h materialsystem/qshaderimage_p.h
        materialsystem/qshaderprogram.cpp materialsystem/qshaderprogram.h materialsystem/qshaderprogram_p.h
        materialsystem/qshaderprogrambuilder.cpp materialsystem/qshaderprogrambuilder.h materialsystem/qshaderprogrambuilder_p.h
        materialsystem/qtechnique.cpp materialsystem/qtechnique.h materialsystem/qtechnique_p.h
        materialsystem/renderpass.cpp materialsystem/renderpass_p.h
        materialsystem/shader.cpp materialsystem/shader_p.h
        materialsystem/shaderbuilder.cpp materialsystem/shaderbuilder_p.h
        materialsystem/shaderdata.cpp materialsystem/shaderdata_p.h
        materialsystem/shaderimage.cpp materialsystem/shaderimage_p.h
        materialsystem/technique.cpp materialsystem/technique_p.h
        materialsystem/techniquemanager.cpp materialsystem/techniquemanager_p.h
        picking/objectpicker.cpp picking/objectpicker_p.h
        picking/pickeventfilter.cpp picking/pickeventfilter_p.h
        picking/qabstractraycaster.cpp picking/qabstractraycaster.h picking/qabstractraycaster_p.h
        picking/qobjectpicker.cpp picking/qobjectpicker.h picking/qobjectpicker_p.h
        picking/qpickevent.cpp picking/qpickevent.h picking/qpickevent_p.h
        picking/qpicklineevent.cpp picking/qpicklineevent.h
        picking/qpickpointevent.cpp picking/qpickpointevent.h
        picking/qpicktriangleevent.cpp picking/qpicktriangleevent.h picking/qpicktriangleevent_p.h
        picking/qraycaster.cpp picking/qraycaster.h
        picking/qraycasterhit.cpp picking/qraycasterhit.h
        picking/qscreenraycaster.cpp picking/qscreenraycaster.h
        picking/raycaster.cpp picking/raycaster_p.h
        qrendererplugin.cpp qrendererplugin_p.h
        qrendererpluginfactory.cpp qrendererpluginfactory_p.h
        qt3drender_global.h qt3drender_global_p.h
        raycasting/boundingsphere.cpp raycasting/boundingsphere_p.h
        raycasting/qabstractcollisionqueryservice.cpp raycasting/qabstractcollisionqueryservice_p.h
        raycasting/qboundingvolume.cpp raycasting/qboundingvolume_p.h
        raycasting/qboundingvolumeprovider.cpp raycasting/qboundingvolumeprovider_p.h
        raycasting/qcollisionqueryresult.cpp raycasting/qcollisionqueryresult_p.h
        raycasting/qray3d.cpp raycasting/qray3d_p.h
        raycasting/qraycastingservice.cpp raycasting/qraycastingservice_p.h
        renderlogging.cpp renderlogging_p.h
        renderstates/genericstate_p.h
        renderstates/qalphacoverage.cpp renderstates/qalphacoverage.h
        renderstates/qalphatest.cpp renderstates/qalphatest.h renderstates/qalphatest_p.h
        renderstates/qblendequation.cpp renderstates/qblendequation.h renderstates/qblendequation_p.h
        renderstates/qblendequationarguments.cpp renderstates/qblendequationarguments.h renderstates/qblendequationarguments_p.h
        renderstates/qclipplane.cpp renderstates/qclipplane.h renderstates/qclipplane_p.h
        renderstates/qcolormask.cpp renderstates/qcolormask.h renderstates/qcolormask_p.h
        renderstates/qcullface.cpp renderstates/qcullface.h renderstates/qcullface_p.h
        renderstates/qdepthrange.cpp renderstates/qdepthrange.h renderstates/qdepthrange_p.h
        renderstates/qdepthtest.cpp renderstates/qdepthtest.h renderstates/qdepthtest_p.h
        renderstates/qdithering.cpp renderstates/qdithering.h
        renderstates/qfrontface.cpp renderstates/qfrontface.h renderstates/qfrontface_p.h
        renderstates/qlinewidth.cpp renderstates/qlinewidth.h renderstates/qlinewidth_p.h
        renderstates/qmultisampleantialiasing.cpp renderstates/qmultisampleantialiasing.h
        renderstates/qnodepthmask.cpp renderstates/qnodepthmask.h
        renderstates/qpointsize.cpp renderstates/qpointsize.h renderstates/qpointsize_p.h
        renderstates/qpolygonoffset.cpp renderstates/qpolygonoffset.h renderstates/qpolygonoffset_p.h
        renderstates/qrastermode.cpp renderstates/qrastermode.h renderstates/qrastermode_p.h
        renderstates/qrenderstate.cpp renderstates/qrenderstate.h renderstates/qrenderstate_p.h
        renderstates/qscissortest.cpp renderstates/qscissortest.h renderstates/qscissortest_p.h
        renderstates/qseamlesscubemap.cpp renderstates/qseamlesscubemap.h
        renderstates/qstencilmask.cpp renderstates/qstencilmask.h renderstates/qstencilmask_p.h
        renderstates/qstenciloperation.cpp renderstates/qstenciloperation.h renderstates/qstenciloperation_p.h
        renderstates/qstenciloperationarguments.cpp renderstates/qstenciloperationarguments.h renderstates/qstenciloperationarguments_p.h
        renderstates/qstenciltest.cpp renderstates/qstenciltest.h renderstates/qstenciltest_p.h
        renderstates/qstenciltestarguments.cpp renderstates/qstenciltestarguments.h renderstates/qstenciltestarguments_p.h
        renderstates/renderstatenode.cpp renderstates/renderstatenode_p.h
        renderstates/renderstates.cpp renderstates/renderstates_p.h
        renderstates/renderstateset.cpp renderstates/renderstateset_p.h
        renderstates/statemask_p.h
        renderstates/statevariant.cpp renderstates/statevariant_p.h
        services/vsyncframeadvanceservice.cpp services/vsyncframeadvanceservice_p.h
        shadergraph/qshaderformat.cpp shadergraph/qshaderformat_p.h
        shadergraph/qshadergenerator.cpp shadergraph/qshadergenerator_p.h
        shadergraph/qshadergraph.cpp shadergraph/qshadergraph_p.h
        shadergraph/qshadergraphloader.cpp shadergraph/qshadergraphloader_p.h
        shadergraph/qshaderlanguage.cpp shadergraph/qshaderlanguage_p.h
        shadergraph/qshadernode.cpp shadergraph/qshadernode_p.h
        shadergraph/qshadernodeport.cpp shadergraph/qshadernodeport_p.h
        shadergraph/qshadernodesloader.cpp shadergraph/qshadernodesloader_p.h
        texture/qabstracttexture.cpp texture/qabstracttexture.h texture/qabstracttexture_p.h
        texture/qabstracttextureimage.cpp texture/qabstracttextureimage.h texture/qabstracttextureimage_p.h
        texture/qpaintedtextureimage.cpp texture/qpaintedtextureimage.h texture/qpaintedtextureimage_p.h
        texture/qtexture.cpp texture/qtexture.h texture/qtexture_p.h
        texture/qtexturedata.cpp texture/qtexturedata.h
        texture/qtexturedataupdate.cpp texture/qtexturedataupdate.h texture/qtexturedataupdate_p.h
        texture/qtexturegenerator.cpp texture/qtexturegenerator_p.h
        texture/qtextureimage.cpp texture/qtextureimage.h texture/qtextureimage_p.h
        texture/qtextureimagedata.cpp texture/qtextureimagedata.h texture/qtextureimagedata_p.h
        texture/qtextureimagedatagenerator.h
        texture/qtexturewrapmode.cpp texture/qtexturewrapmode.h
        texture/texture.cpp texture/texture_p.h
        texture/textureimage.cpp texture/textureimage_p.h
    DEFINES
        BUILD_QT3D_MODULE
    INCLUDE_DIRECTORIES
        backend
        framegraph
        frontend
        geometry
        io
        jobs
        lights
        materialsystem
        picking
        raycasting
        renderstates
        services
        shadergraph
        surfaces
        texture
    LIBRARIES
        Qt::Concurrent
        Qt::CorePrivate
        Qt::3DCorePrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::Core
        Qt::OpenGL
    PRIVATE_MODULE_INTERFACE
        Qt::Concurrent
        Qt::CorePrivate
        Qt::3DCorePrivate
)

# QTransform conflicts
set_target_properties(3DRender PROPERTIES UNITY_BUILD OFF)

# Resources:
set(materialsystem_resource_files
    "materialsystem/prototypes/default.json"
)

qt_internal_add_resource(3DRender "materialsystem"
    PREFIX
        "/"
    BASE
        "materialsystem"
    FILES
        ${materialsystem_resource_files}
)


#### Keys ignored in scope 1:.:.:render.pro:<TRUE>:
# MODULE = "3DRender"

## Scopes:
#####################################################################

# special case begin
# TODO: Prepare for removal, once Platform brings in Threads.
if(NOT TARGET Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG 1)
    find_package(Threads)
    if(TARGET Threads::Threads)
        target_link_libraries(3DRender PRIVATE Threads::Threads)
    endif()
endif()
# special case end

qt_internal_extend_target(3DRender CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

qt_internal_extend_target(3DRender CONDITION QT_FEATURE_vulkan
    SOURCES
        surfaces/vulkaninstance.cpp surfaces/vulkaninstance_p.h
)
