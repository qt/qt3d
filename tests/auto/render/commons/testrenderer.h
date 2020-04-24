/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    qint64 time() const override { return 0; }
    void setTime(qint64 time) override { Q_UNUSED(time) }
    void setAspect(Qt3DRender::QRenderAspect *aspect) override { m_aspect = aspect; }
    void setNodeManagers(Qt3DRender::Render::NodeManagers *m) override
    {
        m_managers = m;
        m_resourceAccessor.reset(new Qt3DRender::Render::ResourceAccessor(this, m_managers));
    }
    void setServices(Qt3DCore::QServiceLocator *services) override { Q_UNUSED(services) }
    void setSurfaceExposed(bool exposed) override { Q_UNUSED(exposed) }
    void setJobsInLastFrame(int jobsInLastFrame) override { Q_UNUSED(jobsInLastFrame) }
    Qt3DRender::QRenderAspect *aspect() const override { return m_aspect; }
    Qt3DRender::Render::NodeManagers *nodeManagers() const override { return m_managers; }
    Qt3DCore::QServiceLocator *services() const override { return nullptr; }
    void initialize() override {}
    void shutdown() override {}
    void releaseGraphicsResources() override {}
    void render() override {}
    void doRender(bool swapBuffers) override { Q_UNUSED(swapBuffers) }
    void cleanGraphicsResources() override {}
    bool isRunning() const override { return true; }
    bool shouldRender() const override { return true; }
    void skipNextFrame() override {}
    void jobsDone(Qt3DCore::QAspectManager *manager) override { Q_UNUSED(manager) }
    QVector<Qt3DCore::QAspectJobPtr> preRenderingJobs() override { return QVector<Qt3DCore::QAspectJobPtr>(); }
    QVector<Qt3DCore::QAspectJobPtr> renderBinJobs() override { return QVector<Qt3DCore::QAspectJobPtr>(); }
    void setSceneRoot(Qt3DRender::Render::Entity *root) override { Q_UNUSED(root) }
    Qt3DRender::Render::Entity *sceneRoot() const override { return nullptr; }
    Qt3DRender::Render::FrameGraphNode *frameGraphRoot() const override { return nullptr; }
    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const override { return nullptr; }
    void setSettings(Qt3DRender::Render::RenderSettings *settings) override { m_settings = settings; }
    Qt3DRender::Render::RenderSettings *settings() const override { return m_settings; }
    void setPendingEvents(const QList<QPair<QObject *, QMouseEvent> > &, const QList<QKeyEvent> &) override { }

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
    bool accessOpenGLTexture(Qt3DCore::QNodeId, QOpenGLTexture **, QMutex **, bool) override { return false; }
    QSharedPointer<Qt3DRender::Render::RenderBackendResourceAccessor> resourceAccessor() const override { return m_resourceAccessor; }

    void loadShader(Qt3DRender::Render::Shader *, Qt3DRender::Render::HShader) override {}

protected:
    Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet m_changes;
    Qt3DRender::QRenderAspect *m_aspect = nullptr;
    Qt3DRender::Render::NodeManagers *m_managers = nullptr;
    QSharedPointer<Qt3DRender::Render::ResourceAccessor> m_resourceAccessor;
    Qt3DRender::Render::RenderSettings *m_settings = nullptr;
};

QT_END_NAMESPACE

#endif // TESTRENDERER_H
