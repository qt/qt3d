// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DRENDERFOREIGN_P_H
#define QT3DQUICK3DRENDERFOREIGN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQml/qqml.h>

#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/qalphacoverage.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qbuffercapture.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qclipplane.h>
#include <Qt3DRender/qcolormask.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qdepthrange.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/qdithering.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qenvironmentlight.h>
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qfrontface.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlevelofdetail.h>
#include <Qt3DRender/qlevelofdetailboundingsphere.h>
#include <Qt3DRender/qlevelofdetailswitch.h>
#include <Qt3DRender/qlinewidth.h>
#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qmultisampleantialiasing.h>
#include <Qt3DRender/qnodepthmask.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qpickingproxy.h>
#include <Qt3DRender/qraycaster.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DRender/qpickingsettings.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qpointsize.h>
#include <Qt3DRender/qpolygonoffset.h>
#include <Qt3DRender/qrendercapture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qsortpolicy.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/qstencilmask.h>
#include <Qt3DRender/qstenciloperation.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/qstenciltest.h>
#include <Qt3DRender/qstenciltestarguments.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qproximityfilter.h>
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/qrastermode.h>
#include <Qt3DRender/qnopicking.h>
#include <Qt3DRender/qshaderimage.h>
#include <Qt3DRender/qsubtreeenabler.h>
#include <Qt3DRender/qrendercapabilities.h>
#include <Qt3DRender/qdebugoverlay.h>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DQuickRender/private/quick3deffect_p.h>
#include <Qt3DQuickRender/private/quick3dlayerfilter_p.h>
#include <Qt3DQuickRender/private/quick3dmaterial_p.h>
#include <Qt3DQuickRender/private/quick3dmemorybarrier_p.h>
#include <Qt3DQuickRender/private/quick3dparameter_p.h>
#include <Qt3DQuickRender/private/quick3drenderpass_p.h>
#include <Qt3DQuickRender/private/quick3drenderpassfilter_p.h>
#include <Qt3DQuickRender/private/quick3drendertargetoutput_p.h>
#include <Qt3DQuickRender/private/quick3dscene_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdata_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdataarray_p.h>
#include <Qt3DQuickRender/private/quick3dstateset_p.h>
#include <Qt3DQuickRender/private/quick3dtechnique_p.h>
#include <Qt3DQuickRender/private/quick3dtechniquefilter_p.h>
#include <Qt3DQuickRender/private/quick3dtexture_p.h>
#include <Qt3DQuickRender/private/quick3dviewport_p.h>
#include <Qt3DQuickRender/private/quick3draycaster_p.h>
#include <Qt3DQuickRender/private/quick3dscreenraycaster_p.h>

QT_BEGIN_NAMESPACE

class QRenderSettingsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderSettings)
    QML_FOREIGN(Qt3DRender::QRenderSettings)
    QML_ADDED_IN_VERSION(2, 0);
};
class QPickingSettingsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PickingSettings)
    QML_FOREIGN(Qt3DRender::QPickingSettings)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderCapabilitiesForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderCapabilities)
    QML_FOREIGN(Qt3DRender::QRenderCapabilities)
    QML_UNCREATABLE("Only available as a property of RenderSettings")
    QML_ADDED_IN_VERSION(2, 15);
};
class QSceneLoaderForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SceneLoader)
    QML_FOREIGN(Qt3DRender::QSceneLoader)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DScene)
    QML_ADDED_IN_VERSION(2, 0);
};
class QEffectForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Effect)
    QML_FOREIGN(Qt3DRender::QEffect)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DEffect)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTechniqueForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Technique)
    QML_FOREIGN(Qt3DRender::QTechnique)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTechnique)
    QML_ADDED_IN_VERSION(2, 0);
};
class QFilterKeyForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(FilterKey)
    QML_FOREIGN(Qt3DRender::QFilterKey)
    QML_ADDED_IN_VERSION(2, 0);
};
class QGraphicsApiFilterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(GraphicsApiFilter)
    QML_FOREIGN(Qt3DRender::QGraphicsApiFilter)
    QML_ADDED_IN_VERSION(2, 0);
};
class QParameterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(QParameter)
    QML_FOREIGN(Qt3DRender::QParameter)
    QML_UNCREATABLE("Quick3D should instantiate Quick3DParameter only")
    QML_ADDED_IN_VERSION(2, 0);
};
class QMaterialForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Material)
    QML_FOREIGN(Qt3DRender::QMaterial)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DMaterial)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderPassForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderPass)
    QML_FOREIGN(Qt3DRender::QRenderPass)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DRenderPass)
    QML_ADDED_IN_VERSION(2, 0);
};
class QShaderProgramForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ShaderProgram)
    QML_FOREIGN(Qt3DRender::QShaderProgram)
    QML_ADDED_IN_VERSION(2, 0);
};
class QShaderProgramBuilderForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ShaderProgramBuilder)
    QML_FOREIGN(Qt3DRender::QShaderProgramBuilder)
    QML_ADDED_IN_VERSION(2, 10);
};
class QShaderDataForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(QShaderData)
    QML_FOREIGN(Qt3DRender::QShaderData)
    QML_UNCREATABLE("Quick3D should instantiate Quick3DShaderData only")
    QML_ADDED_IN_VERSION(2, 0);
};
class QCameraForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Camera)
    QML_FOREIGN(Qt3DRender::QCamera)
    QML_ADDED_IN_VERSION(2, 0);
};
class QCameraLensForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(CameraLens)
    QML_FOREIGN(Qt3DRender::QCameraLens)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureWrapModeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(WrapMode)
    QML_FOREIGN(Qt3DRender::QTextureWrapMode)
    QML_ADDED_IN_VERSION(2, 0);
};
class QAbstractTextureForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture)
    QML_UNCREATABLE("Texture should be created from one of the subclasses")
    QML_FOREIGN(Qt3DRender::QAbstractTexture)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture1DForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture1D)
    QML_FOREIGN(Qt3DRender::QTexture1D)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture1DArrayForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture1DArray)
    QML_FOREIGN(Qt3DRender::QTexture1DArray)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture2DForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture2D)
    QML_FOREIGN(Qt3DRender::QTexture2D)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture2DArrayForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture2DArray)
    QML_FOREIGN(Qt3DRender::QTexture2DArray)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture3DForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture3D)
    QML_FOREIGN(Qt3DRender::QTexture3D)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureCubeMapForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureCubeMap)
    QML_FOREIGN(Qt3DRender::QTextureCubeMap)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureCubeMapArrayForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureCubeMapArray)
    QML_FOREIGN(Qt3DRender::QTextureCubeMapArray)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture2DMultisampleForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture2DMultisample)
    QML_FOREIGN(Qt3DRender::QTexture2DMultisample)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTexture2DMultisampleArrayForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Texture2DMultisampleArray)
    QML_FOREIGN(Qt3DRender::QTexture2DMultisampleArray)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureRectangleForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureRectangle)
    QML_FOREIGN(Qt3DRender::QTextureRectangle)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureBufferForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureBuffer)
    QML_FOREIGN(Qt3DRender::QTextureBuffer)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureLoaderForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureLoader)
    QML_FOREIGN(Qt3DRender::QTextureLoader)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTextureExtension)
    QML_ADDED_IN_VERSION(2, 0);
};
class QAbstractTextureImageForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(QAbstractTextureImage)
    QML_UNCREATABLE("QAbstractTextureImage is abstract")
    QML_FOREIGN(Qt3DRender::QAbstractTextureImage)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTextureImageForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TextureImage)
    QML_FOREIGN(Qt3DRender::QTextureImage)
    QML_ADDED_IN_VERSION(2, 0);
};
class QSharedGLTextureForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SharedGLTexture)
    QML_FOREIGN(Qt3DRender::QSharedGLTexture)
    QML_ADDED_IN_VERSION(2, 13);
};
class QShaderImageForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ShaderImage)
    QML_FOREIGN(Qt3DRender::QShaderImage)
    QML_ADDED_IN_VERSION(2, 14);
};
class QGeometryRendererForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(GeometryRenderer)
    QML_FOREIGN(Qt3DRender::QGeometryRenderer)
    QML_ADDED_IN_VERSION(2, 0);
};
class QLevelOfDetailForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(LevelOfDetail)
    QML_FOREIGN(Qt3DRender::QLevelOfDetail)
    QML_ADDED_IN_VERSION(2, 9);
};
class QLevelOfDetailSwitchForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(LevelOfDetailSwitch)
    QML_FOREIGN(Qt3DRender::QLevelOfDetailSwitch)
    QML_ADDED_IN_VERSION(2, 9);
};
class QLevelOfDetailBoundingSphereForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(levelOfDetailBoundingSphere)
    QML_FOREIGN(Qt3DRender::QLevelOfDetailBoundingSphere)
    QML_ADDED_IN_VERSION(6, 8);
};
class QMeshForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Mesh)
    QML_FOREIGN(Qt3DRender::QMesh)
    QML_ADDED_IN_VERSION(2, 0);
};
class QObjectPickerForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ObjectPicker)
    QML_FOREIGN(Qt3DRender::QObjectPicker)
    QML_ADDED_IN_VERSION(2, 0);
};
class QPickEventForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PickEvent)
    QML_UNCREATABLE("Events cannot be created")
    QML_FOREIGN(Qt3DRender::QPickEvent)
    QML_ADDED_IN_VERSION(2, 0);
};
class Quick3DRayCasterHitForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(Qt3DRender::QRayCasterHit)
    QML_ADDED_IN_VERSION(2, 11);
};
class Quick3DRayCasterHitsForeign {
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(Qt3DRender::QAbstractRayCaster::Hits)
    QML_SEQUENTIAL_CONTAINER(Qt3DRender::QRayCasterHit)
    QML_ADDED_IN_VERSION(2, 11);
};
class QPickingProxyForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PickingProxy)
    QML_FOREIGN(Qt3DRender::QPickingProxy)
    QML_ADDED_IN_VERSION(2, 16);
};
class QComputeCommandForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ComputeCommand)
    QML_FOREIGN(Qt3DRender::QComputeCommand)
    QML_ADDED_IN_VERSION(2, 0);
};
class QLayerForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Layer)
    QML_FOREIGN(Qt3DRender::QLayer)
    QML_ADDED_IN_VERSION(2, 0);
};
class QLayerFilterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(LayerFilter)
    QML_FOREIGN(Qt3DRender::QLayerFilter)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DLayerFilter)
    QML_ADDED_IN_VERSION(2, 0);
};
class QAbstractLightForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Light)
    QML_UNCREATABLE("Light is an abstract base class")
    QML_FOREIGN(Qt3DRender::QAbstractLight)
    QML_ADDED_IN_VERSION(2, 0);
};
class QPointLightForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PointLight)
    QML_FOREIGN(Qt3DRender::QPointLight)
    QML_ADDED_IN_VERSION(2, 0);
};
class QDirectionalLightForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(DirectionalLight)
    QML_FOREIGN(Qt3DRender::QDirectionalLight)
    QML_ADDED_IN_VERSION(2, 0);
};
class QEnvironmentLightForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(EnvironmentLight)
    QML_FOREIGN(Qt3DRender::QEnvironmentLight)
    QML_ADDED_IN_VERSION(2, 9);
};
class QSpotLightForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SpotLight)
    QML_FOREIGN(Qt3DRender::QSpotLight)
    QML_ADDED_IN_VERSION(2, 0);
};
class QCameraSelectorForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(CameraSelector)
    QML_FOREIGN(Qt3DRender::QCameraSelector)
    QML_EXTENDED(Qt3DCore::Quick::Quick3DNode)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderPassFilterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderPassFilter)
    QML_FOREIGN(Qt3DRender::QRenderPassFilter)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DRenderPassFilter)
    QML_ADDED_IN_VERSION(2, 0);
};
class QTechniqueFilterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(TechniqueFilter)
    QML_FOREIGN(Qt3DRender::QTechniqueFilter)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DTechniqueFilter)
    QML_ADDED_IN_VERSION(2, 0);
};
class QViewportForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Viewport)
    QML_FOREIGN(Qt3DRender::QViewport)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DViewport)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderTargetSelectorForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderTargetSelector)
    QML_FOREIGN(Qt3DRender::QRenderTargetSelector)
    QML_ADDED_IN_VERSION(2, 0);
};
class QClearBuffersForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ClearBuffers)
    QML_FOREIGN(Qt3DRender::QClearBuffers)
    QML_ADDED_IN_VERSION(2, 0);
};
class QFrameGraphNodeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(FrameGraphNode)
    QML_FOREIGN(Qt3DRender::QFrameGraphNode)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderStateSetForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderStateSet)
    QML_FOREIGN(Qt3DRender::QRenderStateSet)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DStateSet)
    QML_ADDED_IN_VERSION(2, 0);
};
class QNoDrawForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(NoDraw)
    QML_FOREIGN(Qt3DRender::QNoDraw)
    QML_ADDED_IN_VERSION(2, 0);
};
class QFrustumCullingForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(FrustumCulling)
    QML_FOREIGN(Qt3DRender::QFrustumCulling)
    QML_ADDED_IN_VERSION(2, 0);
};
class QDispatchComputeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(DispatchCompute)
    QML_FOREIGN(Qt3DRender::QDispatchCompute)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderCaptureForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderCapture)
    QML_FOREIGN(Qt3DRender::QRenderCapture)
    QML_ADDED_IN_VERSION(2, 1);
};
class QRenderCaptureReplyForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderCaptureReply)
    QML_UNCREATABLE("RenderCaptureReply is only instantiated by RenderCapture")
    QML_FOREIGN(Qt3DRender::QRenderCaptureReply)
    QML_ADDED_IN_VERSION(2, 1);
};
class QBufferCaptureForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(BufferCapture)
    QML_FOREIGN(Qt3DRender::QBufferCapture)
    QML_ADDED_IN_VERSION(2, 9);
};
class QMemoryBarrierForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(MemoryBarrier)
    QML_FOREIGN(Qt3DRender::QMemoryBarrier)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DMemoryBarrier)
    QML_ADDED_IN_VERSION(2, 9);
};
class QProximityFilterForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ProximityFilter)
    QML_FOREIGN(Qt3DRender::QProximityFilter)
    QML_ADDED_IN_VERSION(2, 10);
};
class QBlitFramebufferForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(BlitFramebuffer)
    QML_FOREIGN(Qt3DRender::QBlitFramebuffer)
    QML_ADDED_IN_VERSION(2, 10);
};
class QSetFenceForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SetFence)
    QML_FOREIGN(Qt3DRender::QSetFence)
    QML_ADDED_IN_VERSION(2, 13);
};
class QWaitFenceForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(WaitFence)
    QML_FOREIGN(Qt3DRender::QWaitFence)
    QML_ADDED_IN_VERSION(2, 13);
};
class QNoPickingForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(NoPicking)
    QML_FOREIGN(Qt3DRender::QNoPicking)
    QML_ADDED_IN_VERSION(2, 14);
};
class QSubtreeEnablerForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SubtreeEnabler)
    QML_FOREIGN(Qt3DRender::QSubtreeEnabler)
    QML_ADDED_IN_VERSION(2, 14);
};
class QDebugOverlayForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(DebugOverlay)
    QML_FOREIGN(Qt3DRender::QDebugOverlay)
    QML_ADDED_IN_VERSION(2, 16);
};
class QRenderTargetOutputForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderTargetOutput)
    QML_FOREIGN(Qt3DRender::QRenderTargetOutput)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderTargetForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderTarget)
    QML_FOREIGN(Qt3DRender::QRenderTarget)
    QML_EXTENDED(Qt3DRender::Render::Quick::Quick3DRenderTargetOutput)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderSurfaceSelectorForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderSurfaceSelector)
    QML_FOREIGN(Qt3DRender::QRenderSurfaceSelector)
    QML_ADDED_IN_VERSION(2, 0);
};
class QSortPolicyForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SortPolicy)
    QML_FOREIGN(Qt3DRender::QSortPolicy)
    QML_ADDED_IN_VERSION(2, 0);
};
class QRenderStateForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RenderState)
    QML_UNCREATABLE("QRenderState is a base class")
    QML_FOREIGN(Qt3DRender::QRenderState)
    QML_ADDED_IN_VERSION(2, 0);
};
class QBlendEquationArgumentsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(BlendEquationArguments)
    QML_FOREIGN(Qt3DRender::QBlendEquationArguments)
    QML_ADDED_IN_VERSION(2, 0);
};
class QBlendEquationForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(BlendEquation)
    QML_FOREIGN(Qt3DRender::QBlendEquation)
    QML_ADDED_IN_VERSION(2, 0);
};
class QAlphaTestForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(AlphaTest)
    QML_FOREIGN(Qt3DRender::QAlphaTest)
    QML_ADDED_IN_VERSION(2, 0);
};
class QDepthRangeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(DepthRange)
    QML_FOREIGN(Qt3DRender::QDepthRange)
    QML_ADDED_IN_VERSION(2, 14);
};
class QDepthTestForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(DepthTest)
    QML_FOREIGN(Qt3DRender::QDepthTest)
    QML_ADDED_IN_VERSION(2, 0);
};
class QMultiSampleAntiAliasingForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(MultiSampleAntiAliasing)
    QML_FOREIGN(Qt3DRender::QMultiSampleAntiAliasing)
    QML_ADDED_IN_VERSION(2, 0);
};
class QNoDepthMaskForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(NoDepthMask)
    QML_FOREIGN(Qt3DRender::QNoDepthMask)
    QML_ADDED_IN_VERSION(2, 0);
};
class QCullFaceForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(CullFace)
    QML_FOREIGN(Qt3DRender::QCullFace)
    QML_ADDED_IN_VERSION(2, 0);
};
class QFrontFaceForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(FrontFace)
    QML_FOREIGN(Qt3DRender::QFrontFace)
    QML_ADDED_IN_VERSION(2, 0);
};
class QStencilTestArgumentsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(StencilTestArguments)
    QML_FOREIGN(Qt3DRender::QStencilTestArguments)
    QML_UNCREATABLE("QStencilTestArguments cannot be instantiated on its own")
    QML_ADDED_IN_VERSION(2, 0);
};
class QStencilTestForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(StencilTest)
    QML_FOREIGN(Qt3DRender::QStencilTest)
    QML_ADDED_IN_VERSION(2, 0);
};
class QScissorTestForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ScissorTest)
    QML_FOREIGN(Qt3DRender::QScissorTest)
    QML_ADDED_IN_VERSION(2, 0);
};
class QDitheringForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Dithering)
    QML_FOREIGN(Qt3DRender::QDithering)
    QML_ADDED_IN_VERSION(2, 0);
};
class QAlphaCoverageForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(AlphaCoverage)
    QML_FOREIGN(Qt3DRender::QAlphaCoverage)
    QML_ADDED_IN_VERSION(2, 0);
};
class QPointSizeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PointSize)
    QML_FOREIGN(Qt3DRender::QPointSize)
    QML_ADDED_IN_VERSION(2, 0);
};
class QPolygonOffsetForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(PolygonOffset)
    QML_FOREIGN(Qt3DRender::QPolygonOffset)
    QML_ADDED_IN_VERSION(2, 0);
};
class QColorMaskForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ColorMask)
    QML_FOREIGN(Qt3DRender::QColorMask)
    QML_ADDED_IN_VERSION(2, 0);
};
class QClipPlaneForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(ClipPlane)
    QML_FOREIGN(Qt3DRender::QClipPlane)
    QML_ADDED_IN_VERSION(2, 0);
};
class QStencilOperationArgumentsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(StencilOperationArguments)
    QML_FOREIGN(Qt3DRender::QStencilOperationArguments)
    QML_UNCREATABLE("StencilOperationArguments cannot be instanciated on its own")
    QML_ADDED_IN_VERSION(2, 0);
};
class QSeamlessCubemapForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(SeamlessCubemap)
    QML_FOREIGN(Qt3DRender::QSeamlessCubemap)
    QML_ADDED_IN_VERSION(2, 0);
};
class QStencilOperationForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(StencilOperation)
    QML_FOREIGN(Qt3DRender::QStencilOperation)
    QML_ADDED_IN_VERSION(2, 0);
};
class QStencilMaskForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(StencilMask)
    QML_FOREIGN(Qt3DRender::QStencilMask)
    QML_ADDED_IN_VERSION(2, 0);
};
class QLineWidthForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(LineWidth)
    QML_FOREIGN(Qt3DRender::QLineWidth)
    QML_ADDED_IN_VERSION(2, 10);
};
class QRasterModeForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(RasterMode)
    QML_FOREIGN(Qt3DRender::QRasterMode)
    QML_ADDED_IN_VERSION(2, 13);
};


QT_END_NAMESPACE

#endif // QT3DQUICK3DRENDERFOREIGN_P_H
