// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RENDERER_H
#define QT3DRENDER_RENDER_RHI_RENDERER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/rendersettings_p.h>
#include <Qt3DRender/private/updateshaderdatatransformjob_p.h>
#include <Qt3DRender/private/framecleanupjob_p.h>
#include <Qt3DRender/private/sendbuffercapturejob_p.h>
#include <Qt3DRender/private/genericlambdajob_p.h>
#include <Qt3DRender/private/shaderbuilder_p.h>
#include <Qt3DRender/private/lightgatherer_p.h>
#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/attachmentpack_p.h>
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>
#include <Qt3DRender/private/filtercompatibletechniquejob_p.h>
#include <Qt3DRender/private/renderqueue_p.h>
#include <Qt3DRender/private/renderercache_p.h>
#include <Qt3DRender/private/renderviewinitializerjob_p.h>

#include <rhi/qrhi.h>

#include <shaderparameterpack_p.h>
#include <logging_p.h>
#include <rhihandle_types_p.h>
#include <renderview_p.h>

#include <QHash>
#include <QMatrix4x4>
#include <QObject>

#include <QOffscreenSurface>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>
#include <QScopedPointer>
#include <QSemaphore>

#include <functional>

#if defined(QT_BUILD_INTERNAL)
class tst_Renderer;
#endif

QT_BEGIN_NAMESPACE

class QSurface;
class QMouseEvent;
class QScreen;

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {

class QCamera;
class QMaterial;
class QShaderProgram;
class QMesh;
class QRenderPass;
class QAbstractShapeMesh;
struct GraphicsApiFilterData;
class QSceneImporter;

namespace DebugRhi {
class CommandExecuter;
}

namespace Render {

class CameraLens;
class FrameGraphNode;
class Material;
class Technique;
class Shader;
class Entity;
class Effect;
class RenderPass;
class RenderStateSet;
class VSyncFrameAdvanceService;
class NodeManagers;
class ResourceAccessor;

using ComputableEntityFilter = FilterEntityByComponentJob<Render::ComputeCommand, Render::Material>;
using ComputableEntityFilterPtr = QSharedPointer<ComputableEntityFilter>;
using RenderableEntityFilter =
        FilterEntityByComponentJob<Render::GeometryRenderer, Render::Material>;
using RenderableEntityFilterPtr = QSharedPointer<RenderableEntityFilter>;

using SynchronizerJobPtr = GenericLambdaJobPtr<std::function<void()>>;
using SynchronizerPostFramePtr =
        GenericLambdaJobAndPostFramePtr<std::function<void()>,
                                        std::function<void(Qt3DCore::QAspectManager *)>>;

namespace Rhi {

class CommandThread;
class SubmissionContext;
class RenderCommand;
struct RHIRenderTarget;
class RHIShader;
class RHIResourceManagers;
class RenderView;
class RHIGraphicsPipeline;
class RHIComputePipeline;
class PipelineUBOSet;

class Q_AUTOTEST_EXPORT Renderer : public AbstractRenderer
{
public:
    explicit Renderer();
    ~Renderer();

    void dumpInfo() const override;
    API api() const override;

    void setRenderDriver(AbstractRenderer::RenderDriver driver) override;
    AbstractRenderer::RenderDriver renderDriver() const override;

    qint64 time() const override;
    void setTime(qint64 time) override;
    void setJobsInLastFrame(int jobsInLastFrame) override;

    void setAspect(QRenderAspect *aspect) override;
    void setNodeManagers(NodeManagers *managers) override;
    void setServices(Qt3DCore::QServiceLocator *services) override;
    void setSurfaceExposed(bool exposed) override;

    QRenderAspect *aspect() const override;
    NodeManagers *nodeManagers() const override;
    Qt3DCore::QServiceLocator *services() const override { return m_services; }

    void initialize() override;
    void shutdown() override;
    void releaseGraphicsResources() override;

    void render(bool swapBuffers = true) override;
    void cleanGraphicsResources() override;

    bool isRunning() const override { return m_running.loadRelaxed(); }

    void setSceneRoot(Entity *sgRoot) override;
    Entity *sceneRoot() const override { return m_renderSceneRoot; }

    FrameGraphNode *frameGraphRoot() const override;
    RenderQueue<RenderView> *renderQueue() { return &m_renderQueue; }

    void markDirty(BackendNodeDirtySet changes, BackendNode *node) override;
    BackendNodeDirtySet dirtyBits() override;

#if defined(QT_BUILD_INTERNAL)
    void clearDirtyBits(BackendNodeDirtySet changes) override;
#endif
    bool shouldRender() const override;
    void skipNextFrame() override;
    void jobsDone(Qt3DCore::QAspectManager *manager) override;

    bool processMouseEvent(QObject *object, QMouseEvent *event) override;
    bool processKeyEvent(QObject *object, QKeyEvent *event) override;

    std::vector<Qt3DCore::QAspectJobPtr> preRenderingJobs() override;
    std::vector<Qt3DCore::QAspectJobPtr> renderBinJobs() override;

    inline FrameCleanupJobPtr frameCleanupJob() const { return m_cleanupJob; }
    inline UpdateShaderDataTransformJobPtr updateShaderDataTransformJob() const
    {
        return m_updateShaderDataTransformJob;
    }
    inline FilterCompatibleTechniqueJobPtr filterCompatibleTechniqueJob() const
    {
        return m_filterCompatibleTechniqueJob;
    }
    inline SynchronizerPostFramePtr introspectShadersJob() const { return m_introspectShaderJob; }
    inline Qt3DCore::QAspectJobPtr bufferGathererJob() const { return m_bufferGathererJob; }
    inline Qt3DCore::QAspectJobPtr textureGathererJob() const { return m_textureGathererJob; }
    inline LightGathererPtr lightGathererJob() const { return m_lightGathererJob; }
    inline RenderableEntityFilterPtr renderableEntityFilterJob() const
    {
        return m_renderableEntityFilterJob;
    }
    inline ComputableEntityFilterPtr computableEntityFilterJob() const
    {
        return m_computableEntityFilterJob;
    }

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;

    void setSettings(RenderSettings *settings) override;
    RenderSettings *settings() const override;
    QOpenGLContext *shareContext() const override;

    inline RHIResourceManagers *rhiResourceManagers() const { return m_RHIResourceManagers; }

    void updateResources();
    void updateTexture(Texture *texture);
    void cleanupTexture(Qt3DCore::QNodeId cleanedUpTextureId);
    void cleanupShader(const Shader *shader);
    void downloadRHIBuffers();

    struct RHIPassInfo
    {
        std::vector<RenderView *> rvs;
        QSurface *surface = nullptr;
        Qt3DCore::QNodeId renderTargetId;
    };

    std::vector<RHIPassInfo> prepareCommandsSubmission(const std::vector<RenderView *> &renderViews);
    bool executeCommandsSubmission(const RHIPassInfo &passInfo);

    // For Scene3D/Scene2D rendering
    void setOpenGLContext(QOpenGLContext *context) override;
    void setRHIContext(QRhi *ctx) override;
    void setDefaultRHIRenderTarget(QRhiRenderTarget *defaultTarget) override;
    void setRHICommandBuffer(QRhiCommandBuffer *commandBuffer) override;
    bool accessOpenGLTexture(Qt3DCore::QNodeId nodeId, QOpenGLTexture **texture, QMutex **lock,
                             bool readonly) override;
    QSharedPointer<RenderBackendResourceAccessor> resourceAccessor() const override;

    const GraphicsApiFilterData *contextInfo() const override;
    SubmissionContext *submissionContext() const;

    inline RenderStateSet *defaultRenderState() const { return m_defaultRenderStateSet; }

    QList<QPair<QObject *, QMouseEvent>> pendingPickingEvents() const;
    QList<QKeyEvent> pendingKeyEvents() const;

    void enqueueRenderView(RenderView *renderView, int submitOrder);
    bool waitUntilReadyToSubmit();

    QVariant executeCommand(const QStringList &args) override;
    void setOffscreenSurfaceHelper(OffscreenSurfaceHelper *) override {};
    QSurfaceFormat format() override;

    struct ViewSubmissionResultData
    {
        ViewSubmissionResultData() : surface(nullptr) { }

        QSurface *surface;
    };

    ViewSubmissionResultData submitRenderViews(const std::vector<RHIPassInfo> &rhiPassesInfo);

    RendererCache<RenderCommand> *cache() { return &m_cache; }
    void setScreen(QScreen *scr) override;
    QScreen *screen() const override;

    float *textureTransform() noexcept { return m_textureTransform; }
    const float *textureTransform() const noexcept { return m_textureTransform; }
#ifdef QT3D_RENDER_UNIT_TESTS
public:
#else

private:
#endif
    Qt3DCore::QServiceLocator *m_services;
    QRenderAspect *m_aspect;
    NodeManagers *m_nodesManager;

    // Frame graph root
    Qt3DCore::QNodeId m_frameGraphRootUuid;

    Entity *m_renderSceneRoot;

    // Fail safe values that we can use if a RenderCommand
    // is missing a shader
    RenderStateSet *m_defaultRenderStateSet;
    ShaderParameterPack m_defaultUniformPack;

    QScopedPointer<SubmissionContext> m_submissionContext;

    RenderQueue<RenderView> m_renderQueue;
    QScopedPointer<VSyncFrameAdvanceService> m_vsyncFrameAdvanceService;

    QSemaphore m_submitRenderViewsSemaphore;
    QSemaphore m_waitForInitializationToBeCompleted;
    QMutex m_hasBeenInitializedMutex;

    QAtomicInt m_running;

    std::vector<Attribute *> m_dirtyAttributes;
    std::vector<Geometry *> m_dirtyGeometry;
    QAtomicInt m_exposed;

    struct DirtyBits
    {
        BackendNodeDirtySet marked; // marked dirty since last job build
        BackendNodeDirtySet remaining; // remaining dirty after jobs have finished
    };
    DirtyBits m_dirtyBits;

    QAtomicInt m_lastFrameCorrect;
    QOpenGLContext *m_glContext;
    QRhi *m_rhiContext;

    qint64 m_time;

    RenderSettings *m_settings;

    UpdateShaderDataTransformJobPtr m_updateShaderDataTransformJob;
    FrameCleanupJobPtr m_cleanupJob;
    SendBufferCaptureJobPtr m_sendBufferCaptureJob;
    FilterCompatibleTechniqueJobPtr m_filterCompatibleTechniqueJob;
    LightGathererPtr m_lightGathererJob;
    RenderableEntityFilterPtr m_renderableEntityFilterJob;
    ComputableEntityFilterPtr m_computableEntityFilterJob;

    QMutex m_pendingRenderCaptureSendRequestsMutex;
    std::vector<Qt3DCore::QNodeId> m_pendingRenderCaptureSendRequests;

    SynchronizerJobPtr m_bufferGathererJob;
    SynchronizerJobPtr m_textureGathererJob;
    SynchronizerPostFramePtr m_introspectShaderJob;

    void lookForDirtyBuffers();
    void lookForDownloadableBuffers();
    void lookForDirtyTextures();
    void reloadDirtyShaders();
    void sendShaderChangesToFrontend(Qt3DCore::QAspectManager *manager);
    void sendTextureChangesToFrontend(Qt3DCore::QAspectManager *manager);
    void sendSetFenceHandlesToFrontend();
    void sendDisablesToFrontend(Qt3DCore::QAspectManager *manager);

    std::vector<HBuffer> m_dirtyBuffers;
    std::vector<Qt3DCore::QNodeId> m_downloadableBuffers;
    std::vector<HShader> m_dirtyShaders;
    std::vector<HTexture> m_dirtyTextures;
    std::vector<QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector>> m_updatedTextureProperties;
    std::vector<Qt3DCore::QNodeId> m_updatedDisableSubtreeEnablers;
    Qt3DCore::QNodeIdVector m_textureIdsToCleanup;
    std::vector<ShaderBuilderUpdate> m_shaderBuilderUpdates;

    bool m_ownedContext;

    RHIResourceManagers *m_RHIResourceManagers;
    QMutex m_offscreenSurfaceMutex;

    QScopedPointer<Qt3DRender::DebugRhi::CommandExecuter> m_commandExecuter;

#ifdef QT_BUILD_INTERNAL
    friend class ::tst_Renderer;
#endif

    QMetaObject::Connection m_contextConnection;
    RendererCache<RenderCommand> m_cache;
    bool m_shouldSwapBuffers;

    std::vector<FrameGraphNode *> m_frameGraphLeaves;
    QScreen *m_screen = nullptr;
    QSharedPointer<ResourceAccessor> m_scene2DResourceAccessor;
    QHash<RenderView *, std::vector<RHIGraphicsPipeline *>> m_rvToGraphicsPipelines;
    QHash<RenderView *, std::vector<RHIComputePipeline *>> m_rvToComputePipelines;
    RenderDriver m_driver = RenderDriver::Qt3D;

    bool m_hasSwapChain = false;

    int m_jobsInLastFrame = 0;

    float m_textureTransform[4];

    bool prepareGeometryInputBindings(const Geometry *geometry, const RHIShader *shader,
                                      QVarLengthArray<QRhiVertexInputBinding, 8> &inputBindings,
                                      QVarLengthArray<QRhiVertexInputAttribute, 8> &rhiAttributes,
                                      QHash<int, int> &attributeNameToBinding);

    void updateGraphicsPipeline(RenderCommand &command, RenderView *rv);
    void updateComputePipeline(RenderCommand &cmd, RenderView *rv,
                               int renderViewIndex);

    void buildGraphicsPipelines(RHIGraphicsPipeline *graphicsPipeline,
                                RenderView *rv,
                                const RenderCommand &command);

    void buildComputePipelines(RHIComputePipeline *computePipeline,
                               RenderView *rv,
                               const RenderCommand &command);

    void cleanupRenderTarget(const Qt3DCore::QNodeId &renderTarget);

    void createRenderTarget(RenderTarget *);
    bool setupRenderTarget(RenderView* rv, RHIGraphicsPipeline* graphicsPipeline, QRhiSwapChain* swapchain);

    bool uploadBuffersForCommand(QRhiCommandBuffer *cb, const RenderView *rv,
                                 RenderCommand &command);

    bool uploadBuffersForCommand(RHIComputePipeline* compute, RenderCommand &command);
    bool uploadBuffersForCommand(RHIGraphicsPipeline* graphics, RenderCommand &command);
    bool uploadUBOsForCommand(QRhiCommandBuffer *cb, const RenderView *rv,
                              const RenderCommand &command);
    bool performCompute(QRhiCommandBuffer *cb, RenderCommand &command);
    bool performDraw(QRhiCommandBuffer *cb, const QRhiViewport &vp, const QRhiScissor *scissor,
                     RenderCommand &command);
    bool setBindingAndShaderResourcesForCommand(QRhiCommandBuffer *cb,
                                                RenderCommand &command,
                                                PipelineUBOSet *uboSet);
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERER_H
