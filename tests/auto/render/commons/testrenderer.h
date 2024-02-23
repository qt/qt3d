// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TESTRENDERER_H
#define TESTRENDERER_H

#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/resourceaccessor_p.h>

QT_BEGIN_NAMESPACE

class TestRenderer : public Qt3DRender::Render::AbstractRenderer
{
public:
    TestRenderer();
    ~TestRenderer();

    void dumpInfo() const override {}
    Qt3DRender::API api() const override { return Qt3DRender::API::OpenGL; }
    void setRenderDriver(AbstractRenderer::RenderDriver) override {};
    AbstractRenderer::RenderDriver renderDriver() const override { return Qt3D; }
    qint64 time() const override { return 0; }
    void setTime(qint64 time) override { Q_UNUSED(time); }
    void setAspect(Qt3DRender::QRenderAspect *aspect) override { m_aspect = aspect; }
    void setNodeManagers(Qt3DRender::Render::NodeManagers *m) override
    {
        m_managers = m;
        m_resourceAccessor.reset(new Qt3DRender::Render::ResourceAccessor(this, m_managers));
    }
    void setServices(Qt3DCore::QServiceLocator *services) override { Q_UNUSED(services); }
    void setSurfaceExposed(bool exposed) override { Q_UNUSED(exposed); }
    void setJobsInLastFrame(int jobsInLastFrame) override { Q_UNUSED(jobsInLastFrame); }
    Qt3DRender::QRenderAspect *aspect() const override { return m_aspect; }
    Qt3DRender::Render::NodeManagers *nodeManagers() const override { return m_managers; }
    Qt3DCore::QServiceLocator *services() const override { return nullptr; }
    void initialize() override {}
    void shutdown() override {}
    void releaseGraphicsResources() override {}
    void render(bool swapBuffers) override { Q_UNUSED(swapBuffers); }
    void cleanGraphicsResources() override {}
    bool isRunning() const override { return true; }
    bool shouldRender() const override { return true; }
    void skipNextFrame() override {}
    void jobsDone(Qt3DCore::QAspectManager *manager) override { Q_UNUSED(manager); }
    std::vector<Qt3DCore::QAspectJobPtr> preRenderingJobs() override { return {}; }
    std::vector<Qt3DCore::QAspectJobPtr> renderBinJobs() override { return {}; }
    void setSceneRoot(Qt3DRender::Render::Entity *root) override { Q_UNUSED(root); }
    Qt3DRender::Render::Entity *sceneRoot() const override { return nullptr; }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const override { return nullptr; }
    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override { return nullptr; }
    void setSettings(Qt3DRender::Render::RenderSettings *settings) override { m_settings = settings; }
    Qt3DRender::Render::RenderSettings *settings() const override { return m_settings; }
    bool processMouseEvent(QObject *, QMouseEvent *) override { return false; }
    bool processKeyEvent(QObject *, QKeyEvent *) override { return false; }

    void markDirty(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes, Qt3DRender::Render::BackendNode *node) override;
    Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet dirtyBits() override;
#if defined(QT_BUILD_INTERNAL)
    void clearDirtyBits(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes) override;
#endif

    void resetDirty();
    QVariant executeCommand(const QStringList &args) override;
    QOpenGLContext *shareContext() const override;
    const Qt3DRender::GraphicsApiFilterData *contextInfo() const override { return  nullptr; }

    void setOffscreenSurfaceHelper(Qt3DRender::Render::OffscreenSurfaceHelper *helper) override;
    QSurfaceFormat format() override;

    void setOpenGLContext(QOpenGLContext *) override {}
    void setRHIContext(QRhi *) override {}
    void setDefaultRHIRenderTarget(QRhiRenderTarget *) override {};
    void setRHICommandBuffer(QRhiCommandBuffer *) override {};
    bool accessOpenGLTexture(Qt3DCore::QNodeId, QOpenGLTexture **, QMutex **, bool) override { return false; }
    QSharedPointer<Qt3DRender::Render::RenderBackendResourceAccessor> resourceAccessor() const override { return m_resourceAccessor; }

protected:
    Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet m_changes;
    Qt3DRender::QRenderAspect *m_aspect = nullptr;
    Qt3DRender::Render::NodeManagers *m_managers = nullptr;
    QSharedPointer<Qt3DRender::Render::ResourceAccessor> m_resourceAccessor;
    Qt3DRender::Render::RenderSettings *m_settings = nullptr;
};

QT_END_NAMESPACE

#endif // TESTRENDERER_H
