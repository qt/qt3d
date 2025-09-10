// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERER_H
#define QT3DRENDER_RENDER_OPENGL_RENDERER_H

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
#include <Qt3DRender/private/filterentitybycomponentjob_p.h>
#include <Qt3DRender/private/filtercompatibletechniquejob_p.h>
#include <Qt3DRender/private/renderqueue_p.h>
#include <Qt3DRender/private/renderercache_p.h>
#include <Qt3DRender/private/renderviewinitializerjob_p.h>
#include <shaderparameterpack_p.h>
#include <logging_p.h>
#include <gl_handle_types_p.h>
#include <glfence_p.h>

#include <QHash>
#include <QMatrix4x4>
#include <QObject>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>
#include <QScopedPointer>
#include <QSemaphore>
#include <QMouseEvent>
#include <QKeyEvent>

#include <functional>

#if defined(QT_BUILD_INTERNAL)
class tst_Renderer;
#endif

QT_BEGIN_NAMESPACE

class QSurface;
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

namespace Debug {
class CommandExecuter;
}

namespace Render {

namespace Profiling {
class FrameProfiler;
}

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
using RenderableEntityFilter = FilterEntityByComponentJob<Render::GeometryRenderer, Render::Material>;
using RenderableEntityFilterPtr = QSharedPointer<RenderableEntityFilter>;

using SynchronizerJobPtr = GenericLambdaJobPtr<std::function<void()>>;
using SynchronizerPostFramePtr = GenericLambdaJobAndPostFramePtr<std::function<void ()>, std::function<void (Qt3DCore::QAspectManager *)>>;
#define CreateSynchronizerPostFramePtr(lambda, postlambda, type) \
    SynchronizerPostFramePtr::create(lambda, postlambda, type, #type)

namespace Debug {
class ImGuiRenderer;
}

namespace OpenGL {

class CommandThread;
class SubmissionContext;
class RenderCommand;
class GLShader;
class GLResourceManagers;
class RenderView;

class Q_AUTOTEST_EXPORT Renderer : public AbstractRenderer
{
public:
    explicit Renderer();
    ~Renderer();

    void dumpInfo() const override;
    API api() const override { return Qt3DRender::API::OpenGL; }

    void setRenderDriver(RenderDriver driver) override;
    RenderDriver renderDriver() const override;

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
    inline UpdateShaderDataTransformJobPtr updateShaderDataTransformJob() const { return m_updateShaderDataTransformJob; }
    inline FilterCompatibleTechniqueJobPtr filterCompatibleTechniqueJob() const { return m_filterCompatibleTechniqueJob; }
    inline SynchronizerPostFramePtr introspectShadersJob() const { return m_introspectShaderJob; }
    inline Qt3DCore::QAspectJobPtr bufferGathererJob() const { return m_bufferGathererJob; }
    inline Qt3DCore::QAspectJobPtr textureGathererJob() const { return m_textureGathererJob; }
    inline LightGathererPtr lightGathererJob() const { return m_lightGathererJob; }
    inline RenderableEntityFilterPtr renderableEntityFilterJob() const { return m_renderableEntityFilterJob; }
    inline ComputableEntityFilterPtr computableEntityFilterJob() const { return m_computableEntityFilterJob; }

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override;

    void setSettings(RenderSettings *settings) override;
    RenderSettings *settings() const override;
    QOpenGLContext *shareContext() const override;

    inline GLResourceManagers *glResourceManagers() const { return m_glResourceManagers; }

    void updateGLResources();
    void updateTexture(Texture *texture);
    void cleanupTexture(Qt3DCore::QNodeId cleanedUpTextureId);
    void cleanupShader(const Shader *shader);
    void downloadGLBuffers();
    void blitFramebuffer(Qt3DCore::QNodeId inputRenderTargetId,
                         Qt3DCore::QNodeId outputRenderTargetId,
                         QRect inputRect,
                         QRect outputRect,
                         GLuint defaultFramebuffer);

    void prepareCommandsSubmission(const std::vector<RenderView *> &renderViews);
    bool executeCommandsSubmission(RenderView *rv);
    bool updateVAOWithAttributes(Geometry *geometry,
                                 const RenderCommand *command,
                                 GLShader *shader,
                                 bool forceUpdate);

    bool requiresVAOAttributeUpdate(Geometry *geometry,
                                    const RenderCommand *command) const;

    // For Scene3D/Scene2D rendering
    void setOpenGLContext(QOpenGLContext *context) override;
    void setRHIContext(QRhi *) override {};
    void setDefaultRHIRenderTarget(QRhiRenderTarget *) override {};
    void setRHICommandBuffer(QRhiCommandBuffer *) override {};
    bool accessOpenGLTexture(Qt3DCore::QNodeId nodeId,
                             QOpenGLTexture **texture,
                             QMutex **lock,
                             bool readonly) override;
    QSharedPointer<RenderBackendResourceAccessor> resourceAccessor() const override;


    const GraphicsApiFilterData *contextInfo() const override;
    SubmissionContext *submissionContext() const;

    inline RenderStateSet *defaultRenderState() const { return m_defaultRenderStateSet; }

    void enqueueRenderView(RenderView *renderView, int submitOrder);
    bool waitUntilReadyToSubmit();

    QVariant executeCommand(const QStringList &args) override;
    void setOffscreenSurfaceHelper(OffscreenSurfaceHelper *helper) override;
    QSurfaceFormat format() override;

    struct ViewSubmissionResultData
    {
        ViewSubmissionResultData()
            : lastBoundFBOId(0)
            , surface(nullptr)
        {}

        uint lastBoundFBOId;
        QSurface *surface;
    };

    ViewSubmissionResultData submitRenderViews(const std::vector<RenderView *> &renderViews);

    RendererCache<RenderCommand> *cache() { return &m_cache; }
    void setScreen(QScreen *scr) override;
    QScreen *screen() const override;

#ifdef QT3D_RENDER_UNIT_TESTS
public:
#else

private:
#endif
    Profiling::FrameProfiler *activeProfiler() const;

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
    QSurfaceFormat m_format;

    RenderQueue<RenderView> m_renderQueue;
    QScopedPointer<VSyncFrameAdvanceService> m_vsyncFrameAdvanceService;

    QSemaphore m_submitRenderViewsSemaphore;
    QSemaphore m_waitForInitializationToBeCompleted;
    QMutex m_hasBeenInitializedMutex;

    QAtomicInt m_running;

    std::vector<Attribute *> m_dirtyAttributes;
    std::vector<Geometry *> m_dirtyGeometry;
    QAtomicInt m_exposed;

    struct DirtyBits {
        BackendNodeDirtySet marked; // marked dirty since last job build
        BackendNodeDirtySet remaining; // remaining dirty after jobs have finished
    };
    DirtyBits m_dirtyBits;

    QAtomicInt m_lastFrameCorrect;
    QOpenGLContext *m_glContext;
    QOpenGLContext *m_shareContext;
    mutable QMutex m_shareContextMutex;

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

    void performDraw(const RenderCommand *command);
    void performCompute(const RenderView *rv, RenderCommand *command);
    void createOrUpdateVAO(RenderCommand *command,
                           HVao *previousVAOHandle,
                           OpenGLVertexArrayObject **vao);

    SynchronizerJobPtr m_bufferGathererJob;
    SynchronizerJobPtr m_vaoGathererJob;
    SynchronizerJobPtr m_textureGathererJob;
    SynchronizerPostFramePtr m_introspectShaderJob;

    void lookForAbandonedVaos();
    void lookForDirtyBuffers();
    void lookForDownloadableBuffers();
    void lookForDirtyTextures();
    void reloadDirtyShaders();
    void sendShaderChangesToFrontend(Qt3DCore::QAspectManager *manager);
    void sendTextureChangesToFrontend(Qt3DCore::QAspectManager *manager);
    void sendSetFenceHandlesToFrontend(Qt3DCore::QAspectManager *manager);
    void sendDisablesToFrontend(Qt3DCore::QAspectManager *manager);

    QMutex m_abandonedVaosMutex;
    std::vector<HVao> m_abandonedVaos;

    std::vector<HBuffer> m_dirtyBuffers;
    std::vector<Qt3DCore::QNodeId> m_downloadableBuffers;
    std::vector<HShader> m_dirtyShaders;
    std::vector<HTexture> m_dirtyTextures;
    std::vector<QPair<Texture::TextureUpdateInfo, Qt3DCore::QNodeIdVector>> m_updatedTextureProperties;
    std::vector<QPair<Qt3DCore::QNodeId, GLFence>> m_updatedSetFences;
    std::vector<Qt3DCore::QNodeId> m_updatedDisableSubtreeEnablers;
    Qt3DCore::QNodeIdVector m_textureIdsToCleanup;
    std::vector<ShaderBuilderUpdate> m_shaderBuilderUpdates;
    Qt3DCore::QNodeIdVector m_lastLoadedShaderIds;

    bool m_ownedContext;

    OffscreenSurfaceHelper *m_offscreenHelper;
    GLResourceManagers *m_glResourceManagers;
    QMutex m_offscreenSurfaceMutex;

    QScopedPointer<Qt3DRender::Debug::CommandExecuter> m_commandExecuter;

    mutable QScopedPointer<Qt3DRender::Render::Profiling::FrameProfiler> m_frameProfiler;

#ifdef QT_BUILD_INTERNAL
    friend class ::tst_Renderer;
#endif

    QMetaObject::Connection m_contextConnection;
    RendererCache<RenderCommand> m_cache;
    bool m_shouldSwapBuffers;
    RenderDriver m_driver = RenderDriver::Qt3D;

    std::vector<FrameGraphNode *> m_frameGraphLeaves;
    QScreen *m_screen = nullptr;
    QSharedPointer<ResourceAccessor> m_scene2DResourceAccessor;

    Debug::ImGuiRenderer *m_imGuiRenderer;
    int m_jobsInLastFrame;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERER_H
