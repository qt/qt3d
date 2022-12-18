// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <Qt3DRender/qpickingproxy.h>
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/qcoreaspect.h>

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
#include <Qt3DRender/private/qrendererpluginfactory_p.h>
#include <Qt3DRender/private/updatelevelofdetailjob_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/pickeventfilter_p.h>
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>

#include <private/qrenderpluginfactory_p.h>
#include <private/qrenderplugin_p.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

#include <QThread>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace {

QString dumpNode(const Qt3DCore::QEntity *n) {
    auto formatNode = [](const Qt3DCore::QNode *n) {
        QString res = QString(QLatin1String("%1{%2}"))
                          .arg(QLatin1String(n->metaObject()->className()))
                          .arg(n->id().id());
        if (!n->objectName().isEmpty())
            res += QString(QLatin1String(" (%1)")).arg(n->objectName());
        if (!n->isEnabled())
            res += QLatin1String(" [D]");
        return res;
    };

    return formatNode(n);
}

QString dumpNodeFilters(const QString &filterType, const QList<Qt3DRender::QFilterKey *> &filters) {
    QString res;

    QStringList kv;
    for (auto filter: filters)
        kv.push_back(QString(QLatin1String("%1: %2")).arg(filter->name(), filter->value().toString()));
    if (kv.size())
        res += QString(QLatin1String("%1 <%2>")).arg(filterType, kv.join(QLatin1String(", ")));

    return res;
}

QStringList dumpSGFilterState(Qt3DRender::Render::TechniqueManager *manager,
                              const Qt3DRender::GraphicsApiFilterData *contextData,
                              const Qt3DCore::QNode *n, int level = 0)
{
    using namespace Qt3DRender;

    QStringList reply;
    const auto *entity = qobject_cast<const Qt3DCore::QEntity *>(n);
    if (entity != nullptr) {
        QString res = dumpNode(entity);
        auto materials = entity->componentsOfType<QMaterial>();
        if (materials.size() && materials.front()->effect()) {
            auto m = materials.front();
            const auto techniques = m->effect()->techniques();
            for (auto t: m->effect()->techniques()) {
                auto apiFilter = t->graphicsApiFilter();
                if (apiFilter) {
                    auto backendTechnique = manager->lookupResource(t->id());
                    if (backendTechnique &&
                        !(*contextData == *backendTechnique->graphicsApiFilter()))
                        continue; // skip technique that doesn't match current renderer
                }

                QStringList filters;
                filters += dumpNodeFilters(QLatin1String("T"), t->filterKeys());

                const auto &renderPasses = t->renderPasses();
                for (auto r: renderPasses)
                    filters += dumpNodeFilters(QLatin1String("RP"), r->filterKeys());

                if (filters.size())
                    res += QLatin1String(" [ %1 ]").arg(filters.join(QLatin1String(" ")));
            }
        }
        reply += res.rightJustified(res.size() + level * 2, QLatin1Char(' '));
        level++;
    }

    const auto children = n->childNodes();
    for (auto *child: children)
        reply += dumpSGFilterState(manager, contextData, child, level);

    return reply;
}

}
namespace Qt3DRender {

#define CreateSynchronizerJobPtr(lambda, type) \
    Render::SynchronizerJobPtr::create(lambda, type, #type)

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
QRenderAspectPrivate::QRenderAspectPrivate(QRenderAspect::SubmissionType submissionType)
    : QAbstractAspectPrivate()
    , m_nodeManagers(nullptr)
    , m_renderer(nullptr)
    , m_initialized(false)
    , m_renderAfterJobs(submissionType == QRenderAspect::Automatic
                        || qEnvironmentVariableIsSet("QT3D_FORCE_SYNCHRONOUS_RENDER"))
    , m_sceneImportersLoaded(false)
    , m_offscreenHelper(nullptr)
    , m_updateTreeEnabledJob(Render::UpdateTreeEnabledJobPtr::create())
    , m_worldTransformJob(Render::UpdateWorldTransformJobPtr::create())
    , m_expandBoundingVolumeJob(Render::ExpandBoundingVolumeJobPtr::create())
    , m_calculateBoundingVolumeJob(Render::CalculateBoundingVolumeJobPtr::create())
    , m_updateWorldBoundingVolumeJob(Render::UpdateWorldBoundingVolumeJobPtr::create())
    , m_updateSkinningPaletteJob(Render::UpdateSkinningPaletteJobPtr::create())
    , m_updateLevelOfDetailJob(Render::UpdateLevelOfDetailJobPtr::create())
    , m_updateEntityLayersJob(Render::UpdateEntityLayersJobPtr::create())
    , m_syncLoadingJobs(CreateSynchronizerJobPtr([] {}, Render::JobTypes::SyncLoadingJobs))
    , m_pickBoundingVolumeJob(Render::PickBoundingVolumeJobPtr::create())
    , m_rayCastingJob(Render::RayCastingJobPtr::create())
    , m_pickEventFilter(new Render::PickEventFilter(this))
    , m_submissionType(submissionType)
{
    m_instances.append(this);

    m_updateWorldBoundingVolumeJob->addDependency(m_worldTransformJob);
    m_updateWorldBoundingVolumeJob->addDependency(m_calculateBoundingVolumeJob);
    m_calculateBoundingVolumeJob->addDependency(m_updateTreeEnabledJob);
    m_expandBoundingVolumeJob->addDependency(m_updateWorldBoundingVolumeJob);
    m_updateLevelOfDetailJob->addDependency(m_expandBoundingVolumeJob);
    m_pickBoundingVolumeJob->addDependency(m_expandBoundingVolumeJob);
    m_pickBoundingVolumeJob->addDependency(m_updateEntityLayersJob);
    m_rayCastingJob->addDependency(m_expandBoundingVolumeJob);
    m_rayCastingJob->addDependency(m_updateEntityLayersJob);
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
    qDeleteAll(m_sceneImporters);
}

QRenderAspectPrivate *QRenderAspectPrivate::findPrivate(Qt3DCore::QAspectEngine *engine)
{
    const QList<QAbstractAspect *> aspects = engine->aspects();
    for (QAbstractAspect* aspect : aspects) {
        QRenderAspect *renderAspect = qobject_cast<QRenderAspect *>(aspect);
        if (renderAspect)
            return static_cast<QRenderAspectPrivate *>(renderAspect->d_ptr.data());
    }
    return nullptr;
}

QRenderAspectPrivate *QRenderAspectPrivate::get(QRenderAspect *q)
{
    return q->d_func();
}

void QRenderAspectPrivate::jobsDone()
{
    m_renderer->jobsDone(m_aspectManager);
}

void QRenderAspectPrivate::frameDone()
{
    m_renderer->setJobsInLastFrame(m_aspectManager->jobsInLastFrame());
    if (m_renderAfterJobs)
        m_renderer->render(true);
}

void QRenderAspectPrivate::createNodeManagers()
{
    m_nodeManagers = new Render::NodeManagers();

    m_updateTreeEnabledJob->setManagers(m_nodeManagers);
    m_worldTransformJob->setManagers(m_nodeManagers);
    m_expandBoundingVolumeJob->setManagers(m_nodeManagers);
    m_calculateBoundingVolumeJob->setManagers(m_nodeManagers);
    m_updateWorldBoundingVolumeJob->setManager(m_nodeManagers->renderNodesManager());
    m_updateSkinningPaletteJob->setManagers(m_nodeManagers);
    m_updateLevelOfDetailJob->setManagers(m_nodeManagers);
    m_updateEntityLayersJob->setManager(m_nodeManagers);
    m_pickBoundingVolumeJob->setManagers(m_nodeManagers);
    m_rayCastingJob->setManagers(m_nodeManagers);

    m_calculateBoundingVolumeJob->setFrontEndNodeManager(m_aspectManager);
}

void QRenderAspectPrivate::onEngineStartup()
{
    Render::Entity *rootEntity = m_nodeManagers->lookupResource<Render::Entity, Render::EntityManager>(m_rootId);
    Q_ASSERT(rootEntity);
    m_renderer->setSceneRoot(rootEntity);

    m_worldTransformJob->setRoot(rootEntity);
    m_expandBoundingVolumeJob->setRoot(rootEntity);
    m_calculateBoundingVolumeJob->setRoot(rootEntity);
    m_updateLevelOfDetailJob->setRoot(rootEntity);
    m_updateSkinningPaletteJob->setRoot(rootEntity);
    m_updateTreeEnabledJob->setRoot(rootEntity);
    m_pickBoundingVolumeJob->setRoot(rootEntity);
    m_rayCastingJob->setRoot(rootEntity);

    // Ensures all skeletons are loaded before we try to update them
    m_updateSkinningPaletteJob->addDependency(m_syncLoadingJobs);

    // make sure bv job in core aspect runs before the one in render aspect
    if (m_aspectManager) {
        auto *coreAspect = qobject_cast<Qt3DCore::QCoreAspect *>(m_aspectManager->aspect(&Qt3DCore::QCoreAspect::staticMetaObject));
        Q_ASSERT(coreAspect);
        m_calculateBoundingVolumeJob->addDependency(coreAspect->calculateBoundingVolumeJob());

        auto bvJob = qSharedPointerCast<Qt3DCore::CalculateBoundingVolumeJob>(coreAspect->calculateBoundingVolumeJob());
        bvJob->addWatcher(m_calculateBoundingVolumeJob);
    }
}

void QRenderAspectPrivate::onEngineAboutToShutdown()
{
    if (m_aspectManager) {
        auto *coreAspect = qobject_cast<Qt3DCore::QCoreAspect *>(m_aspectManager->aspect(&Qt3DCore::QCoreAspect::staticMetaObject));
        Q_ASSERT(coreAspect);

        auto bvJob = qSharedPointerCast<Qt3DCore::CalculateBoundingVolumeJob>(coreAspect->calculateBoundingVolumeJob());
        bvJob->removeWatcher(m_calculateBoundingVolumeJob);
    }
}

/*! \internal */
void QRenderAspectPrivate::registerBackendTypes()
{
    Q_Q(QRenderAspect);

    qRegisterMetaType<Qt3DCore::QBuffer*>();
    qRegisterMetaType<Qt3DCore::QJoint*>();

    qRegisterMetaType<Qt3DRender::QEffect*>();
    qRegisterMetaType<Qt3DRender::QFrameGraphNode *>();
    qRegisterMetaType<Qt3DRender::QCamera*>();
    qRegisterMetaType<Qt3DRender::QShaderProgram*>();
    qRegisterMetaType<Qt3DRender::QViewport*>();

    q->registerBackendType<Qt3DCore::QEntity>(QSharedPointer<Render::RenderEntityFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<Qt3DCore::QTransform>(QSharedPointer<Render::NodeFunctor<Render::Transform, Render::TransformManager> >::create(m_renderer));

    q->registerBackendType<Qt3DRender::QCameraLens>(QSharedPointer<Render::CameraLensFunctor>::create(m_renderer, q));
    q->registerBackendType<QLayer>(QSharedPointer<Render::NodeFunctor<Render::Layer, Render::LayerManager> >::create(m_renderer));
    q->registerBackendType<QLevelOfDetail>(QSharedPointer<Render::NodeFunctor<Render::LevelOfDetail, Render::LevelOfDetailManager> >::create(m_renderer));
    q->registerBackendType<QLevelOfDetailSwitch>(QSharedPointer<Render::NodeFunctor<Render::LevelOfDetail, Render::LevelOfDetailManager> >::create(m_renderer));
    q->registerBackendType<QSceneLoader>(QSharedPointer<Render::RenderSceneFunctor>::create(m_renderer, m_nodeManagers->sceneManager()));
    q->registerBackendType<QRenderTarget>(QSharedPointer<Render::RenderTargetFunctor>::create(m_renderer, m_nodeManagers->renderTargetManager()));
    q->registerBackendType<QRenderTargetOutput>(QSharedPointer<Render::NodeFunctor<Render::RenderTargetOutput, Render::AttachmentManager> >::create(m_renderer));
    q->registerBackendType<QRenderSettings>(QSharedPointer<Render::RenderSettingsFunctor>::create(m_renderer));
    q->registerBackendType<QRenderState>(QSharedPointer<Render::NodeFunctor<Render::RenderStateNode, Render::RenderStateManager> >::create(m_renderer));

    // Geometry + Compute
    q->registerBackendType<Qt3DCore::QAttribute>(QSharedPointer<Render::NodeFunctor<Render::Attribute, Render::AttributeManager> >::create(m_renderer));
    q->registerBackendType<Qt3DCore::QBuffer>(QSharedPointer<Render::BufferFunctor>::create(m_renderer, m_nodeManagers->bufferManager()));
    q->registerBackendType<QComputeCommand>(QSharedPointer<Render::NodeFunctor<Render::ComputeCommand, Render::ComputeCommandManager> >::create(m_renderer));
    q->registerBackendType<QGeometry>(QSharedPointer<Render::NodeFunctor<Render::Geometry, Render::GeometryManager> >::create(m_renderer));
    q->registerBackendType<QGeometryRenderer>(QSharedPointer<Render::GeometryRendererFunctor>::create(m_renderer, m_nodeManagers->geometryRendererManager()));
    q->registerBackendType<QPickingProxy>(QSharedPointer<Render::PickingProxyFunctor>::create(m_renderer, m_nodeManagers->pickingProxyManager()));
    q->registerBackendType<Qt3DCore::QArmature>(QSharedPointer<Render::NodeFunctor<Render::Armature, Render::ArmatureManager>>::create(m_renderer));
    q->registerBackendType<Qt3DCore::QAbstractSkeleton>(QSharedPointer<Render::SkeletonFunctor>::create(m_renderer, m_nodeManagers->skeletonManager(), m_nodeManagers->jointManager()));
    q->registerBackendType<Qt3DCore::QJoint>(QSharedPointer<Render::JointFunctor>::create(m_renderer, m_nodeManagers->jointManager(), m_nodeManagers->skeletonManager()));

    // Textures
    q->registerBackendType<QAbstractTexture>(QSharedPointer<Render::TextureFunctor>::create(m_renderer, m_nodeManagers->textureManager()));
    q->registerBackendType<QAbstractTextureImage>(QSharedPointer<Render::TextureImageFunctor>::create(m_renderer,
                                                                                                      m_nodeManagers->textureImageManager()));

    // Material system
    q->registerBackendType<QEffect>(QSharedPointer<Render::NodeFunctor<Render::Effect, Render::EffectManager> >::create(m_renderer));
    q->registerBackendType<QFilterKey>(QSharedPointer<Render::NodeFunctor<Render::FilterKey, Render::FilterKeyManager> >::create(m_renderer));
    q->registerBackendType<QAbstractLight>(QSharedPointer<Render::RenderLightFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QEnvironmentLight>(QSharedPointer<Render::NodeFunctor<Render::EnvironmentLight, Render::EnvironmentLightManager> >::create(m_renderer));
    q->registerBackendType<QMaterial>(QSharedPointer<Render::NodeFunctor<Render::Material, Render::MaterialManager> >::create(m_renderer));
    q->registerBackendType<QParameter>(QSharedPointer<Render::NodeFunctor<Render::Parameter, Render::ParameterManager> >::create(m_renderer));
    q->registerBackendType<QRenderPass>(QSharedPointer<Render::NodeFunctor<Render::RenderPass, Render::RenderPassManager> >::create(m_renderer));
    q->registerBackendType<QShaderData>(QSharedPointer<Render::RenderShaderDataFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QShaderProgram>(QSharedPointer<Render::ShaderFunctor>::create(m_renderer, m_nodeManagers->shaderManager()));
    q->registerBackendType<QShaderProgramBuilder>(QSharedPointer<Render::NodeFunctor<Render::ShaderBuilder, Render::ShaderBuilderManager> >::create(m_renderer));
    q->registerBackendType<QTechnique>(QSharedPointer<Render::TechniqueFunctor>::create(m_renderer, m_nodeManagers));
    q->registerBackendType<QShaderImage>(QSharedPointer<Render::NodeFunctor<Render::ShaderImage, Render::ShaderImageManager>>::create(m_renderer));

    // Framegraph
    q->registerBackendType<QFrameGraphNode>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::FrameGraphNode, QFrameGraphNode> >::create(m_renderer));
    q->registerBackendType<QCameraSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::CameraSelector, QCameraSelector> >::create(m_renderer));
    q->registerBackendType<QClearBuffers>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ClearBuffers, QClearBuffers> >::create(m_renderer));
    q->registerBackendType<QDispatchCompute>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::DispatchCompute, QDispatchCompute> >::create(m_renderer));
    q->registerBackendType<QFrustumCulling>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::FrustumCulling, QFrustumCulling> >::create(m_renderer));
    q->registerBackendType<QLayerFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::LayerFilterNode, QLayerFilter> >::create(m_renderer));
    q->registerBackendType<QNoDraw>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::NoDraw, QNoDraw> >::create(m_renderer));
    q->registerBackendType<QRenderPassFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderPassFilter, QRenderPassFilter> >::create(m_renderer));
    q->registerBackendType<QRenderStateSet>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::StateSetNode, QRenderStateSet> >::create(m_renderer));
    q->registerBackendType<QRenderSurfaceSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderSurfaceSelector, QRenderSurfaceSelector> >::create(m_renderer));
    q->registerBackendType<QRenderTargetSelector>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderTargetSelector, QRenderTargetSelector> >::create(m_renderer));
    q->registerBackendType<QSortPolicy>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SortPolicy, QSortPolicy> >::create(m_renderer));
    q->registerBackendType<QTechniqueFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::TechniqueFilter, QTechniqueFilter> >::create(m_renderer));
    q->registerBackendType<QViewport>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ViewportNode, QViewport> >::create(m_renderer));
    q->registerBackendType<QRenderCapture>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::RenderCapture, QRenderCapture> >::create(m_renderer));
    q->registerBackendType<QBufferCapture>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::BufferCapture, QBufferCapture> >::create(m_renderer));
    q->registerBackendType<QMemoryBarrier>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::MemoryBarrier, QMemoryBarrier> >::create(m_renderer));
    q->registerBackendType<QProximityFilter>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::ProximityFilter, QProximityFilter> >::create(m_renderer));
    q->registerBackendType<QBlitFramebuffer>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::BlitFramebuffer, QBlitFramebuffer> >::create(m_renderer));
    q->registerBackendType<QSetFence>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SetFence, QSetFence> >::create(m_renderer));
    q->registerBackendType<QWaitFence>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::WaitFence, QWaitFence> >::create(m_renderer));
    q->registerBackendType<QNoPicking>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::NoPicking, QNoPicking> >::create(m_renderer));
    q->registerBackendType<QSubtreeEnabler>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::SubtreeEnabler, QSubtreeEnabler> >::create(m_renderer));
    q->registerBackendType<QDebugOverlay>(QSharedPointer<Render::FrameGraphNodeFunctor<Render::DebugOverlay, QDebugOverlay> >::create(m_renderer));

    // Picking
    q->registerBackendType<QObjectPicker>(QSharedPointer<Render::NodeFunctor<Render::ObjectPicker, Render::ObjectPickerManager> >::create(m_renderer));
    q->registerBackendType<QRayCaster>(QSharedPointer<Render::NodeFunctor<Render::RayCaster, Render::RayCasterManager> >::create(m_renderer));
    q->registerBackendType<QScreenRayCaster>(QSharedPointer<Render::NodeFunctor<Render::RayCaster, Render::RayCasterManager> >::create(m_renderer));

    // Plugins
    for (const QString &plugin : std::as_const(m_pluginConfig))
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
    unregisterBackendType<Qt3DCore::QAttribute>();
    unregisterBackendType<Qt3DCore::QBuffer>();
    unregisterBackendType<QComputeCommand>();
    unregisterBackendType<QGeometry>();
    unregisterBackendType<QGeometryRenderer>();
    unregisterBackendType<QPickingProxy>();
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
    for (Render::QRenderPlugin *plugin : std::as_const(m_renderPlugins))
        plugin->unregisterBackendTypes(q);
}

void QRenderAspectPrivate::registerBackendType(const QMetaObject &obj,
                                               const QBackendNodeMapperPtr &functor)
{
    Q_Q(QRenderAspect);
    q->registerBackendType(obj, functor);
}

/*!
 * \enum QRenderAspect::SubmissionType
 *
 * \value Automatic
 *        The QRenderAspect takes care of submitting rendering commands to the
 *        GPU.
 * \value Manual
 *        The user will take care of telling the QRenderAspect when is the
 *        appropriate time to submit the rendering commands to the GPU.
 */

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a parent. This constructor will set the submission type to
 * Automatic.
 * \param parent
 */
QRenderAspect::QRenderAspect(QObject *parent)
    : QRenderAspect(QRenderAspect::Automatic, parent)
{
}

/*!
 * The constructor creates a new QRenderAspect::QRenderAspect instance with the
 * specified \a parent. The \a submissionType specifies whether the
 * RenderAspect is in charge of performing the rendering submission or if the
 * user will take care of it.
 * \param parent
 */
QRenderAspect::QRenderAspect(QRenderAspect::SubmissionType submissionType,
                             QObject *parent)
    : Qt3DCore::QAbstractAspect(*new QRenderAspectPrivate(submissionType),
                                parent)
{

}

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

std::vector<Qt3DCore::QAspectJobPtr> QRenderAspect::jobsToExecute(qint64 time)
{
    using namespace Render;

    Q_D(QRenderAspect);
    d->m_renderer->setTime(time);

#if defined(QT3D_RENDER_DUMP_BACKEND_NODES)
    d->m_renderer->dumpInfo();
#endif

    // Create jobs that will get executed by the threadpool
    std::vector<QAspectJobPtr> jobs;

    // 1 GeometryJobs, SceneLoaderJobs, LoadTextureJobs
    // 2 CalculateBoundingVolumeJob (depends on LoadBuffer)
    // 3 WorldTransformJob
    // 4 UpdateBoundingVolume, FramePreparationJob (depend on WorlTransformJob)
    // 5 RenderViewJobs
    // 6 PickBoundingVolumeJob
    // 7 Cleanup Job (depends on RV)

    // Ensure we have a settings object. It may get deleted by the call to
    // QChangeArbiter::syncChanges() that happens just before the render aspect is
    // asked for jobs to execute (this function). If that is the case, the RenderSettings will
    // be null and we should not generate any jobs.
    if (d->m_renderer->isRunning() && d->m_renderer->settings()) {
        NodeManagers *manager = d->m_nodeManagers;
        d->m_syncLoadingJobs->removeDependency(QWeakPointer<QAspectJob>());
        d->m_calculateBoundingVolumeJob->removeDependency(QWeakPointer<QAspectJob>());
        d->m_updateLevelOfDetailJob->setFrameGraphRoot(d->m_renderer->frameGraphRoot());

        // Launch skeleton loader jobs once all loading jobs have completed.
        const QList<Render::HSkeleton> skeletonsToLoad =
                manager->skeletonManager()->takeDirtySkeletons(Render::SkeletonManager::SkeletonDataDirty);
        for (const auto &skeletonHandle : skeletonsToLoad) {
            auto loadSkeletonJob = Render::LoadSkeletonJobPtr::create(skeletonHandle);
            loadSkeletonJob->setNodeManagers(manager);
            d->m_syncLoadingJobs->addDependency(loadSkeletonJob);
            jobs.push_back(loadSkeletonJob);
        }

        // TO DO: Have 2 jobs queue
        // One for urgent jobs that are mandatory for the rendering of a frame
        // Another for jobs that can span across multiple frames (Scene/Mesh loading)
        const std::vector<Render::LoadSceneJobPtr> sceneJobs = manager->sceneManager()->takePendingSceneLoaderJobs();
        if (!sceneJobs.empty() && !d->m_sceneImportersLoaded) {
            d->loadSceneImporters();
        }
        for (const Render::LoadSceneJobPtr &job : sceneJobs) {
            job->setNodeManagers(d->m_nodeManagers);
            job->setSceneImporters(d->m_sceneImporters);
            jobs.push_back(job);
        }

        Qt3DCore::moveAtEnd(jobs, d->createGeometryRendererJobs());

        Qt3DCore::moveAtEnd(jobs, d->createPreRendererJobs());

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

        const AbstractRenderer::BackendNodeDirtySet dirtyBitsForFrame = d->m_renderer->dirtyBits();

        // Create the jobs to build the frame
        const bool entitiesEnabledDirty = dirtyBitsForFrame & AbstractRenderer::EntityEnabledDirty;
        if (entitiesEnabledDirty)
            jobs.push_back(d->m_updateTreeEnabledJob);

        if (entitiesEnabledDirty ||
            dirtyBitsForFrame & AbstractRenderer::TransformDirty) {
            jobs.push_back(d->m_worldTransformJob);
            jobs.push_back(d->m_updateWorldBoundingVolumeJob);
        }

        if (entitiesEnabledDirty ||
            dirtyBitsForFrame & AbstractRenderer::GeometryDirty ||
            dirtyBitsForFrame & AbstractRenderer::BuffersDirty) {
            jobs.push_back(d->m_calculateBoundingVolumeJob);
        }

        if (entitiesEnabledDirty ||
            dirtyBitsForFrame & AbstractRenderer::GeometryDirty ||
            dirtyBitsForFrame & AbstractRenderer::TransformDirty) {
            jobs.push_back(d->m_expandBoundingVolumeJob);
        }

        // TO DO: Conditionally add if skeletons dirty
        jobs.push_back(d->m_syncLoadingJobs);
        d->m_updateSkinningPaletteJob->setDirtyJoints(manager->jointManager()->dirtyJoints());
        jobs.push_back(d->m_updateSkinningPaletteJob);
        jobs.push_back(d->m_updateLevelOfDetailJob);

        // Rebuild Entity Layers list if layers are dirty
        const bool layersDirty = dirtyBitsForFrame & AbstractRenderer::LayersDirty;
        if (layersDirty)
            jobs.push_back(d->m_updateEntityLayersJob);

        Qt3DCore::moveAtEnd(jobs, d->m_renderer->renderBinJobs());
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
            if (args.front() == QLatin1String("filterstates")) {
                const auto activeContextInfo = d->m_renderer->contextInfo();
                QString res = QLatin1String("Active Graphics API: ") + activeContextInfo->toString() + QLatin1String("\n");
                res += QLatin1String("Render Views:\n  ") + Qt3DRender::QFrameGraphNodePrivate::get(fg)->dumpFrameGraphFilterState().join(QLatin1String("\n  ")) + QLatin1String("\n");
                res += QLatin1String("Scene Graph:\n  ") + dumpSGFilterState(d->m_nodeManagers->techniqueManager(), activeContextInfo, d->m_root).join(QLatin1String("\n  "));
                return res;
            }
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
    d->onEngineStartup();
}

QStringList QRenderAspect::dependencies() const
{
    return { QLatin1String("core") };
}

void QRenderAspect::onRegistered()
{
    // Create a renderer each time as this is destroyed in onUnregistered below. If
    // using a threaded renderer, this blocks until the render thread has been created
    // and started.
    Q_D(QRenderAspect);
    d->createNodeManagers();

    // Load proper Renderer class based on Qt configuration preferences
    d->m_renderer = d->loadRendererPlugin();
    Q_ASSERT(d->m_renderer);
    d->m_renderer->setScreen(d->m_screen);
    d->m_renderer->setAspect(this);
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
        d->services()->eventFilterService()->registerEventFilter(d->m_pickEventFilter.data(), 1024);
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

    if (d->m_aspectManager)
        d->services()->eventFilterService()->unregisterEventFilter(d->m_pickEventFilter.data());

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

std::vector<Qt3DCore::QAspectJobPtr> QRenderAspectPrivate::createGeometryRendererJobs() const
{
    Render::GeometryRendererManager *geomRendererManager = m_nodeManagers->geometryRendererManager();
    const QList<QNodeId> dirtyGeometryRenderers = geomRendererManager->dirtyGeometryRenderers();
    std::vector<QAspectJobPtr> dirtyGeometryRendererJobs;
    dirtyGeometryRendererJobs.reserve(dirtyGeometryRenderers.size());

    for (const QNodeId &geoRendererId : dirtyGeometryRenderers) {
        Render::HGeometryRenderer geometryRendererHandle = geomRendererManager->lookupHandle(geoRendererId);
        if (!geometryRendererHandle.isNull()) {
            auto job = Render::LoadGeometryJobPtr::create(geometryRendererHandle);
            job->setNodeManagers(m_nodeManagers);
            dirtyGeometryRendererJobs.push_back(job);
        }
    }

    return dirtyGeometryRendererJobs;
}

std::vector<QAspectJobPtr> QRenderAspectPrivate::createPreRendererJobs() const
{
    if (!m_renderer)
        return {};

    auto jobs = m_renderer->preRenderingJobs();

    // Set values on picking jobs
    Render::RenderSettings *renderSetting = m_renderer->settings();
    if (renderSetting != nullptr) {
        m_pickBoundingVolumeJob->setRenderSettings(renderSetting);
        m_pickBoundingVolumeJob->setFrameGraphRoot(m_renderer->frameGraphRoot());

        m_rayCastingJob->setRenderSettings(renderSetting);
        m_rayCastingJob->setFrameGraphRoot(m_renderer->frameGraphRoot());
    }

    jobs.push_back(m_pickBoundingVolumeJob);
    jobs.push_back(m_rayCastingJob);

    return jobs;
}

void QRenderAspectPrivate::loadSceneImporters()
{
    const QStringList keys = QSceneImportFactory::keys();
    for (const QString &key : keys) {
        QSceneImporter *sceneIOHandler = QSceneImportFactory::create(key, QStringList());
        if (sceneIOHandler != nullptr)
            m_sceneImporters.append(sceneIOHandler);
    }
    m_sceneImportersLoaded = true;
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
        Render::AbstractRenderer *renderer = Render::QRendererPluginFactory::create(key);
        if (renderer)
            return renderer;
    }
    const QByteArray targetKeyName = targetKey.toLatin1();
    qFatal("Unable to find renderer plugin for %s", targetKeyName.constData());
    return nullptr;
}

bool QRenderAspectPrivate::processMouseEvent(QObject *obj, QMouseEvent *event)
{
    Render::RenderSettings *renderSetting = m_renderer->settings();
    if (!renderSetting)
        return false;

    if (m_renderer->processMouseEvent(obj, event))
        return true;
    m_pickBoundingVolumeJob->processMouseEvent(obj, event);

    return false;
}

bool QRenderAspectPrivate::processKeyEvent(QObject *obj, QKeyEvent *event)
{
    Q_UNUSED(obj);

    if (m_renderer->processKeyEvent(obj, event))
        return true;

    return false;
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

QList<QString> QRenderAspectPrivate::m_pluginConfig;
QMutex QRenderAspectPrivate::m_pluginLock;
QList<QRenderAspectPrivate *> QRenderAspectPrivate::m_instances;

void QRenderAspectPrivate::configurePlugin(const QString &plugin)
{
    QMutexLocker lock(&m_pluginLock);
    if (!m_pluginConfig.contains(plugin)) {
        m_pluginConfig.append(plugin);

        for (QRenderAspectPrivate *instance : std::as_const(m_instances))
            instance->loadRenderPlugin(plugin);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("render", QT_PREPEND_NAMESPACE(Qt3DRender), QRenderAspect)

#include "moc_qrenderaspect.cpp"
