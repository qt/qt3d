/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qrenderaspect.h"
#include "qrenderaspect_p.h"

#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>

#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlayerfilter.h>
#include <Qt3DRender/qlevelofdetail.h>
#include <Qt3DRender/qlevelofdetailswitch.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/qclearbuffers.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qrenderstateset.h>
#include <Qt3DRender/qnodraw.h>
#include <Qt3DRender/qnopicking.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qraycaster.h>
#include <Qt3DRender/qscreenraycaster.h>
#include <Qt3DRender/qfrustumculling.h>
#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/qenvironmentlight.h>
#include <Qt3DRender/qdispatchcompute.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qrendercapture.h>
#include <Qt3DRender/qbuffercapture.h>
#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/qproximityfilter.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/qblitframebuffer.h>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/qshaderimage.h>
#include <Qt3DRender/qsubtreeenabler.h>
#include <Qt3DRender/qdebugoverlay.h>
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qskeletonloader.h>

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/cameraselectornode_p.h>
#include <Qt3DRender/private/layerfilternode_p.h>
#include <Qt3DRender/private/cameralens_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/shaderdata_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/rendertargetselectornode_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/viewportnode_p.h>
#include <Qt3DRender/private/rendertarget_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DRender/private/clearbuffers_p.h>
#include <Qt3DRender/private/sortpolicy_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/nodefunctor_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/textureimage_p.h>
#include <Qt3DRender/private/statesetnode_p.h>
#include <Qt3DRender/private/nodraw_p.h>
#include <Qt3DRender/private/nopicking_p.h>
#include <Qt3DRender/private/vsyncframeadvanceservice_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/private/raycaster_p.h>
#include <Qt3DRender/private/boundingvolumedebug_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/calcgeometrytrianglevolumes_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/buffermanager_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/loadgeometryjob_p.h>
#include <Qt3DRender/private/qsceneimportfactory_p.h>
#include <Qt3DRender/private/qsceneimporter_p.h>
#include <Qt3DRender/private/frustumculling_p.h>
#include <Qt3DRender/private/light_p.h>
#include <Qt3DRender/private/environmentlight_p.h>
#include <Qt3DRender/private/dispatchcompute_p.h>
#include <Qt3DRender/private/computecommand_p.h>
#include <Qt3DRender/private/rendersurfaceselector_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/rendercapture_p.h>
#include <Qt3DRender/private/buffercapture_p.h>
#include <Qt3DRender/private/technique_p.h>
#include <Qt3DRender/private/offscreensurfacehelper_p.h>
#include <Qt3DRender/private/memorybarrier_p.h>
#include <Qt3DRender/private/shaderbuilder_p.h>
#include <Qt3DRender/private/blitframebuffer_p.h>
#include <Qt3DRender/private/subtreeenabler_p.h>
#include <Qt3DRender/private/armature_p.h>
#include <Qt3DRender/private/skeleton_p.h>
#include <Qt3DRender/private/joint_p.h>
#include <Qt3DRender/private/loadskeletonjob_p.h>
#include <Qt3DRender/private/proximityfilter_p.h>
#include <Qt3DRender/private/setfence_p.h>
#include <Qt3DRender/private/waitfence_p.h>
#include <Qt3DRender/private/shaderimage_p.h>
#include <Qt3DRender/private/debugoverlay_p.h>

#include <private/qrenderpluginfactory_p.h>
#include <private/qrenderplugin_p.h>

#include <Qt3DRender/private/qrendererpluginfactory_p.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

#include <QThread>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QRenderAspect
 * \inheaderfile Qt3DRender/QRenderAspect
 * \brief The QRenderAspect class.
 * \since 5.7
 * \inmodule Qt3DRender
 */

/*!
    \namespace Qt3DRender::Render
    \inmodule Qt3DRender

    \brief Namespace used for accessing the classes
    Renderer and QRenderPlugin.
*/
/*! \internal */
QRenderAspectPrivate::QRenderAspectPrivate(QRenderAspect::RenderType type)
    : QAbstractAspectPrivate()
    , m_nodeManagers(nullptr)
    , m_renderer(nullptr)
    , m_initialized(false)
    , m_renderAfterJobs(false)
    , m_renderType(type)
    , m_offscreenHelper(nullptr)
{
    m_instances.append(this);
    loadSceneParsers();
    if (m_renderType == QRenderAspect::Threaded && !QOpenGLContext::supportsThreadedOpenGL()) {
        m_renderType = QRenderAspect::Synchronous;
        m_renderAfterJobs = true;
    }
}

/*! \internal */
QRenderAspectPrivate::~QRenderAspectPrivate()
{
    // The renderer should have been shutdown as part of onUnregistered().
    // If it still exists then this aspect is being deleted before the aspect
    // engine is finished with it.
    if (m_renderer != nullptr)
        qWarning() << Q_FUNC_INFO << "The renderer should have been deleted when reaching this point (this warning may be normal when running tests)";
    delete m_nodeManagers;
    m_instances.removeAll(this);
    qDeleteAll(m_sceneImporter);
}

QRenderAspectPrivate *QRenderAspectPrivate::findPrivate(Qt3DCore::QAspectEngine *engine)
{
    const QVector<QAbstractAspect*> aspects = engine->aspects();
    for (QAbstractAspect* aspect : aspects) {
        QRenderAspect *renderAspect = qobject_cast<QRenderAspect *>(aspect);
        if (renderAspect)
            return static_cast<QRenderAspectPrivate *>(renderAspect->d_ptr.data());
    }
    return nullptr;
}

void QRenderAspectPrivate::syncDirtyFrontEndNode(QNode *node, QBackendNode *backend, bool firstTime) const
{
    Render::BackendNode *renderBackend = static_cast<Render::BackendNode *>(backend);
    renderBackend->syncFromFrontEnd(node, firstTime);
}

void QRenderAspectPrivate::jobsDone()
{
    m_renderer->jobsDone(m_aspectManager);
}

void QRenderAspectPrivate::frameDone()
{
    m_renderer->setJobsInLastFrame(m_aspectManager->jobsInLastFrame());
    if (m_renderAfterJobs)
        m_renderer->doRender(true);
}

/*! \internal */
void QRenderAspectPrivate::registerBackendTypes()
{
    Q_Q(QRenderAspect);

    qRegisterMetaType<Qt3DRender::QBuffer*>();
    qRegisterMetaType<Qt3DRender::QEffect*>();
    qRegisterMetaType<Qt3DRender::QFrameGraphNode *>();
    qRegisterMetaType<Qt3DRender::QCamera*>();
    qRegisterMetaType<Qt3DRender::QShaderProgram*>();
    qRegisterMetaType<Qt3DRender::QViewport*>();
    qRegisterMetaType<Qt3DCore::QJoint*>();

    q->registerBackendType<Qt3DCore::QEntity, true>(QSharedPointer<Render::RenderEntityFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<Qt3DCore::QTransform, true>(QSharedPointer<Render::NodeFunctor<Render::Transform, Render::TransformManager> >::create(m_renderer));

    q->registerBackendType<Qt3DRender::QCameraLens, true>(QSharedPointer<Render::CameraLensFunctor>::create(m_renderer, q));
    q->registerBackendType<QLayer, true>(QSharedPointer<Render::NodeFunctor<Render::Layer, Render::LayerManager> >::create(m_renderer));
    q->registerBackendType<QLevelOfDetail, true>(QSharedPointer<Render::NodeFunctor<Render::LevelOfDetail, Render::LevelOfDetailManager> >::create(m_renderer));
    q->registerBackendType<QLevelOfDetailSwitch, true>(QSharedPointer<Render::NodeFunctor<Render::LevelOfDetail, Render::LevelOfDetailManager> >::create(m_renderer));
    q->registerBackendType<QSceneLoader, true>(QSharedPointer<Render::RenderSceneFunctor>::create(m_renderer, m_nodeManagers->sceneManager()));
    q->registerBackendType<QRenderTarget, true>(QSharedPointer<Render::NodeFunctor<Render::RenderTarget, Render::RenderTargetManager> >::create(m_renderer));
    q->registerBackendType<QRenderTargetOutput, true>(QSharedPointer<Render::NodeFunctor<Render::RenderTargetOutput, Render::AttachmentManager> >::create(m_renderer));
    q->registerBackendType<QRenderSettings, true>(QSharedPointer<Render::RenderSettingsFunctor>::create(m_renderer));
    q->registerBackendType<QRenderState, true>(QSharedPointer<Render::NodeFunctor<Render::RenderStateNode, Render::RenderStateManager> >::create(m_renderer));

    // Geometry + Compute
    q->registerBackendType<QAttribute, true>(QSharedPointer<Render::NodeFunctor<Render::Attribute, Render::AttributeManager> >::create(m_renderer));
    q->registerBackendType<QBuffer, true>(QSharedPointer<Render::BufferFunctor>::create(m_renderer, m_nodeManagers->bufferManager()));
    q->registerBackendType<QComputeCommand, true>(QSharedPointer<Render::NodeFunctor<Render::ComputeCommand, Render::ComputeCommandManager> >::create(m_renderer));
    q->registerBackendType<QGeometry, true>(QSharedPointer<Render::NodeFunctor<Render::Geometry, Render::GeometryManager> >::create(m_renderer));
    q->registerBackendType<QGeometryRenderer, true>(QSharedPointer<Render::GeometryRendererFunctor>::create(m_renderer, m_nodeManagers->geometryRendererManager()));
    q->registerBackendType<Qt3DCore::QArmature, true>(QSharedPointer<Render::NodeFunctor<Render::Armature, Render::ArmatureManager>>::create(m_renderer));
    q->registerBackendType<Qt3DCore::QAbstractSkeleton, true>(QSharedPointer<Render::SkeletonFunctor>::create(m_renderer, m_nodeManagers->skeletonManager(), m_nodeManagers->jointManager()));
    q->registerBackendType<Qt3DCore::QJoint, true>(QSharedPointer<Render::JointFunctor>::create(m_renderer, m_nodeManagers->jointManager(), m_nodeManagers->skeletonManager()));

    // Textures
    q->registerBackendType<QAbstractTexture, true>(QSharedPointer<Render::TextureFunctor>::create(m_renderer, m_nodeManagers->textureManager()));
    q->registerBackendType<QAbstractTextureImage, true>(QSharedPointer<Render::TextureImageFunctor>::create(m_renderer,
                                                                                                      m_nodeManagers->textureImageManager()));

    // Material system
    q->registerBackendType<QEffect, true>(QSharedPointer<Render::NodeFunctor<Render::Effect, Render::EffectManager> >::create(m_renderer));
    q->registerBackendType<QFilterKey, true>(QSharedPointer<Render::NodeFunctor<Render::FilterKey, Render::FilterKeyManager> >::create(m_renderer));
    q->registerBackendType<QAbstractLight, true>(QSharedPointer<Render::RenderLightFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QEnvironmentLight, true>(QSharedPointer<Render::NodeFunctor<Render::EnvironmentLight, Render::EnvironmentLightManager> >::create(m_renderer));
    q->registerBackendType<QMaterial, true>(QSharedPointer<Render::NodeFunctor<Render::Material, Render::MaterialManager> >::create(m_renderer));
    q->registerBackendType<QParameter, true>(QSharedPointer<Render::NodeFunctor<Render::Parameter, Render::ParameterManager> >::create(m_renderer));
    q->registerBackendType<QRenderPass, true>(QSharedPointer<Render::NodeFunctor<Render::RenderPass, Render::RenderPassManager> >::create(m_renderer));
    q->registerBackendType<QShaderData, true>(QSharedPointer<Render::RenderShaderDataFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QShaderProgram, true>(QSharedPointer<Render::ShaderFunctor>::create(m_renderer, m_nodeManagers->shaderManager()));
    q->registerBackendType<QShaderProgramBuilder, true>(QSharedPointer<Render::NodeFunctor<Render::ShaderBuilder, Render::ShaderBuilderManager> >::create(m_renderer));
    q->registerBackendType<QTechnique, true>(QSharedPointer<Render::TechniqueFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QShaderImage, true>(QSharedPointer<Render::NodeFunctor<Render::ShaderImage, Render::ShaderImageManager>>::create(m_renderer));

    // Framegraph
    q->registerBackendType<QFrameGraphNode, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::FrameGraphNode, QFrameGraphNode> >::create(m_renderer));
    q->registerBackendType<QCameraSelector, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector> >::create(m_renderer));
    q->registerBackendType<QClearBuffers, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ClearBuffers, QClearBuffers> >::create(m_renderer));
    q->registerBackendType<QDispatchCompute, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::DispatchCompute, QDispatchCompute> >::create(m_renderer));
    q->registerBackendType<QFrustumCulling, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::FrustumCulling, QFrustumCulling> >::create(m_renderer));
    q->registerBackendType<QLayerFilter, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter> >::create(m_renderer));
    q->registerBackendType<QNoDraw, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::NoDraw, QNoDraw> >::create(m_renderer));
    q->registerBackendType<QRenderPassFilter, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter> >::create(m_renderer));
    q->registerBackendType<QRenderStateSet, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::StateSetNode, QRenderStateSet> >::create(m_renderer));
    q->registerBackendType<QRenderSurfaceSelector, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderSurfaceSelector, QRenderSurfaceSelector> >::create(m_renderer));
    q->registerBackendType<QRenderTargetSelector, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector> >::create(m_renderer));
    q->registerBackendType<QSortPolicy, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SortPolicy, QSortPolicy> >::create(m_renderer));
    q->registerBackendType<QTechniqueFilter, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter> >::create(m_renderer));
    q->registerBackendType<QViewport, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport> >::create(m_renderer));
    q->registerBackendType<QRenderCapture, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderCapture, QRenderCapture> >::create(m_renderer));
    q->registerBackendType<QBufferCapture, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::BufferCapture, QBufferCapture> >::create(m_renderer));
    q->registerBackendType<QMemoryBarrier, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::MemoryBarrier, QMemoryBarrier> >::create(m_renderer));
    q->registerBackendType<QProximityFilter, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ProximityFilter, QProximityFilter> >::create(m_renderer));
    q->registerBackendType<QBlitFramebuffer, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::BlitFramebuffer, QBlitFramebuffer> >::create(m_renderer));
    q->registerBackendType<QSetFence, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SetFence, QSetFence> >::create(m_renderer));
    q->registerBackendType<QWaitFence, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::WaitFence, QWaitFence> >::create(m_renderer));
    q->registerBackendType<QNoPicking, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::NoPicking, QNoPicking> >::create(m_renderer));
    q->registerBackendType<QSubtreeEnabler, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SubtreeEnabler, QSubtreeEnabler> >::create(m_renderer));
    q->registerBackendType<QDebugOverlay, true>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::DebugOverlay, QDebugOverlay> >::create(m_renderer));

    // Picking
    q->registerBackendType<QObjectPicker, true>(QSharedPointer<Render::NodeFunctor<Render::ObjectPicker, Render::ObjectPickerManager> >::create(m_renderer));
    q->registerBackendType<QRayCaster, true>(QSharedPointer<Render::NodeFunctor<Render::RayCaster, Render::RayCasterManager> >::create(m_renderer));
    q->registerBackendType<QScreenRayCaster, true>(QSharedPointer<Render::NodeFunctor<Render::RayCaster, Render::RayCasterManager> >::create(m_renderer));

    // Plugins
    for (const QString &plugin : qAsConst(m_pluginConfig))
        loadRenderPlugin(plugin);
}

/*! \internal */
void QRenderAspectPrivate::unregisterBackendTypes()
{
    Q_Q(QRenderAspect);
    unregisterBackendType<Qt3DCore::QEntity>();
    unregisterBackendType<Qt3DCore::QTransform>();

    unregisterBackendType<Qt3DRender::QCameraLens>();
    unregisterBackendType<QLayer>();
    unregisterBackendType<QSceneLoader>();
    unregisterBackendType<QRenderTarget>();
    unregisterBackendType<QRenderTargetOutput>();
    unregisterBackendType<QRenderSettings>();
    unregisterBackendType<QRenderState>();

    // Geometry + Compute
    unregisterBackendType<QAttribute>();
    unregisterBackendType<QBuffer>();
    unregisterBackendType<QComputeCommand>();
    unregisterBackendType<QGeometry>();
    unregisterBackendType<QGeometryRenderer>();
    unregisterBackendType<Qt3DCore::QArmature>();
    unregisterBackendType<Qt3DCore::QAbstractSkeleton>();
    unregisterBackendType<Qt3DCore::QJoint>();

    // Textures
    unregisterBackendType<QAbstractTexture>();
    unregisterBackendType<QAbstractTextureImage>();

    // Material system
    unregisterBackendType<QEffect>();
    unregisterBackendType<QFilterKey>();
    unregisterBackendType<QAbstractLight>();
    unregisterBackendType<QEnvironmentLight>();
    unregisterBackendType<QMaterial>();
    unregisterBackendType<QParameter>();
    unregisterBackendType<QRenderPass>();
    unregisterBackendType<QShaderData>();
    unregisterBackendType<QShaderProgram>();
    unregisterBackendType<QShaderProgramBuilder>();
    unregisterBackendType<QTechnique>();
    unregisterBackendType<QShaderImage>();

    // Framegraph
    unregisterBackendType<QCameraSelector>();
    unregisterBackendType<QClearBuffers>();
    unregisterBackendType<QDispatchCompute>();
    unregisterBackendType<QFrustumCulling>();
    unregisterBackendType<QLayerFilter>();
    unregisterBackendType<QNoDraw>();
    unregisterBackendType<QRenderPassFilter>();
    unregisterBackendType<QRenderStateSet>();
    unregisterBackendType<QRenderSurfaceSelector>();
    unregisterBackendType<QRenderTargetSelector>();
    unregisterBackendType<QSortPolicy>();
    unregisterBackendType<QTechniqueFilter>();
    unregisterBackendType<QViewport>();
    unregisterBackendType<QRenderCapture>();
    unregisterBackendType<QBufferCapture>();
    unregisterBackendType<QMemoryBarrier>();
    unregisterBackendType<QSetFence>();
    unregisterBackendType<QWaitFence>();
    unregisterBackendType<QSubtreeEnabler>();
    unregisterBackendType<QDebugOverlay>();

    // Picking
    unregisterBackendType<QObjectPicker>();
    unregisterBackendType<QRayCaster>();
    unregisterBackendType<QScreenRayCaster>();

    // Plugins
    for (Render::QRenderPlugin *plugin : qAsConst(m_renderPlugins))
        plugin->unregisterBackendTypes(q);
}

void QRenderAspectPrivate::registerBackendType(const QMetaObject &obj,
                                               const QBackendNodeMapperPtr &functor)
{
    Q_Q(QRenderAspect);
    q->registerBackendType(obj, functor);
}

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a parent.
 * \param parent
 */
QRenderAspect::QRenderAspect(QObject *parent)
    : QRenderAspect(Threaded, parent) {}

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a type and \a parent.
 * \param type
 * \param parent
 */
QRenderAspect::QRenderAspect(QRenderAspect::RenderType type, QObject *parent)
    : QRenderAspect(*new QRenderAspectPrivate(type), parent) {}

/*! \internal */
QRenderAspect::QRenderAspect(QRenderAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    setObjectName(QStringLiteral("Render Aspect"));
}

/*! \internal */
QRenderAspect::~QRenderAspect()
{
}

// Called by Scene3DRenderer only
void QRenderAspectPrivate::renderInitialize(QOpenGLContext *context)
{
    if (m_renderer->api() == Render::AbstractRenderer::OpenGL)
        m_renderer->setOpenGLContext(context);
    m_renderer->initialize();
}

/*! \internal */
void QRenderAspectPrivate::renderSynchronous(bool swapBuffers)
{
    m_renderer->doRender(swapBuffers);
}

/*
 * \internal
 * Only called when rendering with QtQuick 2 and a Scene3D item
 */
void QRenderAspectPrivate::renderShutdown()
{
    m_renderer->shutdown();
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspect::jobsToExecute(qint64 time)
{
    Q_D(QRenderAspect);
    d->m_renderer->setTime(time);

#if defined(QT3D_RENDER_DUMP_BACKEND_NODES)
    d->m_renderer->dumpInfo();
#endif

    // Create jobs that will get executed by the threadpool
    QVector<QAspectJobPtr> jobs;

    // 1 LoadBufferJobs, GeometryJobs, SceneLoaderJobs, LoadTextureJobs
    // 2 CalculateBoundingVolumeJob (depends on LoadBuffer)
    // 3 WorldTransformJob
    // 4 UpdateBoundingVolume, FramePreparationJob (depend on WorlTransformJob)
    // 5 CalcGeometryTriangleVolumes (frame preparation job), RenderViewJobs
    // 6 PickBoundingVolumeJob
    // 7 Cleanup Job (depends on RV)

    // Ensure we have a settings object. It may get deleted by the call to
    // QChangeArbiter::syncChanges() that happens just before the render aspect is
    // asked for jobs to execute (this function). If that is the case, the RenderSettings will
    // be null and we should not generate any jobs.
    if (d->m_renderer->isRunning() && d->m_renderer->settings()) {

        Render::NodeManagers *manager = d->m_renderer->nodeManagers();
        QAspectJobPtr loadingJobSync = d->m_renderer->syncLoadingJobs();
        loadingJobSync->removeDependency(QWeakPointer<QAspectJob>());

        // Launch skeleton loader jobs once all loading jobs have completed.
        const QVector<Render::HSkeleton> skeletonsToLoad =
                manager->skeletonManager()->takeDirtySkeletons(Render::SkeletonManager::SkeletonDataDirty);
        for (const auto &skeletonHandle : skeletonsToLoad) {
            auto loadSkeletonJob = Render::LoadSkeletonJobPtr::create(skeletonHandle);
            loadSkeletonJob->setNodeManagers(manager);
            loadingJobSync->addDependency(loadSkeletonJob);
            jobs.append(loadSkeletonJob);
        }

        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        const QVector<Render::LoadSceneJobPtr> sceneJobs = manager->sceneManager()->takePendingSceneLoaderJobs();
        for (const Render::LoadSceneJobPtr &job : sceneJobs) {
            job->setNodeManagers(d->m_nodeManagers);
            job->setSceneImporters(d->m_sceneImporter);
            jobs.append(job);
        }

        const QVector<QAspectJobPtr> geometryJobs = d->createGeometryRendererJobs();
        jobs.append(geometryJobs);

        const QVector<QAspectJobPtr> preRenderingJobs = d->m_renderer->preRenderingJobs();
        jobs.append(preRenderingJobs);

        // Don't spawn any rendering jobs, if the renderer decides to skip this frame
        // Note: this only affects rendering jobs (jobs that load buffers,
        // perform picking,... must still be run)
        if (!d->m_renderer->shouldRender()) {
            d->m_renderer->skipNextFrame();
            QThread::msleep(1);
            return jobs;
        }

        // Traverse the current framegraph and create jobs to populate
        // RenderBins with RenderCommands
        // All jobs needed to create the frame and their dependencies are set by
        // renderBinJobs()
        const QVector<QAspectJobPtr> renderBinJobs = d->m_renderer->renderBinJobs();
        jobs.append(renderBinJobs);
    }
    return jobs;
}

QVariant QRenderAspect::executeCommand(const QStringList &args)
{
    Q_D(QRenderAspect);

    if (args.size() == 1) {
        Render::RenderSettings *settings = d->m_renderer->settings();
        auto *droot = static_cast<Qt3DCore::QEntityPrivate *>(Qt3DCore::QNodePrivate::get(d->m_root));
        auto *fg = qobject_cast<Qt3DRender::QFrameGraphNode *>(droot->m_scene->lookupNode(settings->activeFrameGraphID()));
        if (fg) {
            if (args.front() == QLatin1String("framegraph"))
                return Qt3DRender::QFrameGraphNodePrivate::get(fg)->dumpFrameGraph();
            if (args.front() == QLatin1String("framepaths"))
                return Qt3DRender::QFrameGraphNodePrivate::get(fg)->dumpFrameGraphPaths().join(QLatin1String("\n"));
        }
        if (args.front() == QLatin1String("scenegraph"))
            return droot->dumpSceneGraph();
    }

    return d->m_renderer->executeCommand(args);
}

void QRenderAspect::onEngineStartup()
{
    Q_D(QRenderAspect);
    if (d->m_renderAfterJobs)   // synchronous rendering but using QWindow
        d->m_renderer->initialize();
    Render::NodeManagers *managers = d->m_renderer->nodeManagers();
    Render::Entity *rootEntity = managers->lookupResource<Render::Entity, Render::EntityManager>(rootEntityId());
    Q_ASSERT(rootEntity);
    d->m_renderer->setSceneRoot(rootEntity);
}

void QRenderAspect::onRegistered()
{
    // Create a renderer each time as this is destroyed in onUnregistered below. If
    // using a threaded renderer, this blocks until the render thread has been created
    // and started.
    Q_D(QRenderAspect);
    d->m_nodeManagers = new Render::NodeManagers();

    // Load proper Renderer class based on Qt configuration preferences
    d->m_renderer = d->loadRendererPlugin();
    Q_ASSERT(d->m_renderer);
    d->m_renderer->setScreen(d->m_screen);
    d->m_renderer->setNodeManagers(d->m_nodeManagers);

    // Create a helper for deferring creation of an offscreen surface used during cleanup
    // to the main thread, after we know what the surface format in use is.
    d->m_offscreenHelper = new Render::OffscreenSurfaceHelper(d->m_renderer);
    d->m_offscreenHelper->moveToThread(QCoreApplication::instance()->thread());
    d->m_renderer->setOffscreenSurfaceHelper(d->m_offscreenHelper);

    // Register backend types now that we have a renderer
    d->registerBackendTypes();

    if (!d->m_initialized) {

        // Register the VSyncFrameAdvanceService to drive the aspect manager loop
        // depending on the vsync
        if (d->m_aspectManager) {
            QAbstractFrameAdvanceService *advanceService = d->m_renderer->frameAdvanceService();
            if (advanceService)
                d->services()->registerServiceProvider(Qt3DCore::QServiceLocator::FrameAdvanceService,
                                                       advanceService);
        }

        if (d->services())
            d->m_renderer->setServices(d->services());
        d->m_initialized = true;
    }

    if (d->m_aspectManager)
        d->m_renderer->registerEventFilter(d->services()->eventFilterService());
}

void QRenderAspect::onUnregistered()
{
    Q_D(QRenderAspect);
    if (d->m_renderer) {
        // Request the renderer shuts down. In the threaded renderer case, the
        // Renderer destructor is the synchronization point where we wait for the
        // thread to join (see below).
        d->m_renderer->shutdown();
    }

    d->unregisterBackendTypes();

    d->m_renderer->releaseGraphicsResources();

    delete d->m_nodeManagers;
    d->m_nodeManagers = nullptr;

    // Waits for the render thread to join (if using threaded renderer)
    delete d->m_renderer;
    d->m_renderer = nullptr;

    // Queue the offscreen surface helper for deletion on the main thread.
    // That will take care of deleting the offscreen surface itself.
    d->m_offscreenHelper->deleteLater();
    d->m_offscreenHelper = nullptr;
}

QVector<Qt3DCore::QAspectJobPtr> QRenderAspectPrivate::createGeometryRendererJobs()
{
    Render::GeometryRendererManager *geomRendererManager = m_nodeManagers->geometryRendererManager();
    const QVector<QNodeId> dirtyGeometryRenderers = geomRendererManager->dirtyGeometryRenderers();
    QVector<QAspectJobPtr> dirtyGeometryRendererJobs;
    dirtyGeometryRendererJobs.reserve(dirtyGeometryRenderers.size());

    for (const QNodeId geoRendererId : dirtyGeometryRenderers) {
        Render::HGeometryRenderer geometryRendererHandle = geomRendererManager->lookupHandle(geoRendererId);
        if (!geometryRendererHandle.isNull()) {
            auto job = Render::LoadGeometryJobPtr::create(geometryRendererHandle);
            job->setNodeManagers(m_nodeManagers);
            dirtyGeometryRendererJobs.push_back(job);
        }
    }

    return dirtyGeometryRendererJobs;
}

void QRenderAspectPrivate::loadSceneParsers()
{
    const QStringList keys = QSceneImportFactory::keys();
    for (const QString &key : keys) {
        QSceneImporter *sceneIOHandler = QSceneImportFactory::create(key, QStringList());
        if (sceneIOHandler != nullptr)
            m_sceneImporter.append(sceneIOHandler);
    }
}

Render::AbstractRenderer *QRenderAspectPrivate::loadRendererPlugin()
{
    // Note: for now we load the first renderer plugin that is successfully loaded
    // In the future we might want to offer the user a way to hint at which renderer
    // plugin would best be loaded

    const QByteArray envTarget = qgetenv("QT3D_RENDERER");
    const QString targetKey = !envTarget.isEmpty() ? QString::fromLatin1(envTarget) : QStringLiteral("opengl");
    const QStringList keys = Render::QRendererPluginFactory::keys();
    for (const QString &key : keys) {
        if (key != targetKey)
            continue;
        Render::AbstractRenderer *renderer = Render::QRendererPluginFactory::create(key, m_renderType);
        if (renderer)
            return renderer;
    }
    const QByteArray targetKeyName = targetKey.toLatin1();
    qFatal("Unable to find renderer plugin for %s", targetKeyName.constData());
    return nullptr;
}

void QRenderAspectPrivate::loadRenderPlugin(const QString &pluginName)
{
    Q_Q(QRenderAspect);
    const QStringList keys = Render::QRenderPluginFactory::keys();
    if (!keys.contains(pluginName))
        return;

    if (m_pluginConfig.contains(pluginName) && !m_loadedPlugins.contains(pluginName)) {
        Render::QRenderPlugin *plugin
                = Render::QRenderPluginFactory::create(pluginName, QStringList());
        if (plugin != nullptr) {
            m_loadedPlugins.append(pluginName);
            m_renderPlugins.append(plugin);
            plugin->registerBackendTypes(q, m_renderer);
        }
    }
}

QVector<QString> QRenderAspectPrivate::m_pluginConfig;
QMutex QRenderAspectPrivate::m_pluginLock;
QVector<QRenderAspectPrivate *> QRenderAspectPrivate::m_instances;

void QRenderAspectPrivate::configurePlugin(const QString &plugin)
{
    QMutexLocker lock(&m_pluginLock);
    if (!m_pluginConfig.contains(plugin)) {
        m_pluginConfig.append(plugin);

        for (QRenderAspectPrivate *instance : qAsConst(m_instances))
            instance->loadRenderPlugin(plugin);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("render", QT_PREPEND_NAMESPACE(Qt3DRender), QRenderAspect)
