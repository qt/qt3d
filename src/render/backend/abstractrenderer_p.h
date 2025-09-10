// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H
#define QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H

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

#include <QtCore/qflags.h>
#include <QtCore/qmutex.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qrenderapi.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>
#include <QtGui/qsurfaceformat.h>

#include <qopenglcontext.h>

QT_BEGIN_NAMESPACE

class QSurface;
class QSize;
class QScreen;
class QOpenGLTexture;
class QMouseEvent;
class QKeyEvent;
class QRhi;
class QRhiRenderTarget;
class QRhiCommandBuffer;

namespace Qt3DCore {
class QAbstractFrameAdvanceService;
class QAbstractAspectJobManager;
class QServiceLocator;
class QAspectManager;
}

namespace Qt3DRender {

class QRenderAspect;
struct GraphicsApiFilterData;

namespace Render {

class NodeManagers;
class Entity;
class FrameGraphNode;
class RenderSettings;
class BackendNode;
class OffscreenSurfaceHelper;
class Shader;
class RenderBackendResourceAccessor;
class Q_3DRENDERSHARED_PRIVATE_EXPORT AbstractRenderer
{
public:
    virtual ~AbstractRenderer() {}

    // Changes made to backend nodes are reported to the Renderer
    enum BackendNodeDirtyFlag {
        TransformDirty      = 1 << 0,
        MaterialDirty       = 1 << 1,
        GeometryDirty       = 1 << 2,
        ComputeDirty        = 1 << 3,
        ParameterDirty      = 1 << 4,
        FrameGraphDirty     = 1 << 5,
        EntityEnabledDirty  = 1 << 6,
        BuffersDirty        = 1 << 7,
        TexturesDirty       = 1 << 8,
        ShadersDirty        = 1 << 9,
        SkeletonDataDirty   = 1 << 10,
        JointDirty          = 1 << 11,
        LayersDirty         = 1 << 12,
        TechniquesDirty     = 1 << 13,
        LightsDirty         = 1 << 15,
        AllDirty            = 0xffffff
    };
    Q_DECLARE_FLAGS(BackendNodeDirtySet, BackendNodeDirtyFlag)

    enum RenderDriver {
        Qt3D,
        Scene3D,
    };

    virtual void dumpInfo() const = 0;

    virtual API api() const = 0;
    virtual void setRenderDriver(RenderDriver driver) = 0;
    virtual RenderDriver renderDriver() const = 0;

    virtual qint64 time() const = 0;
    virtual void setTime(qint64 time) = 0;
    virtual void setJobsInLastFrame(int jobsInLastFrame) = 0;

    virtual void setAspect(QRenderAspect *aspect) = 0;
    virtual void setNodeManagers(NodeManagers *managers) = 0;
    virtual void setServices(Qt3DCore::QServiceLocator *services) = 0;
    virtual void setSurfaceExposed(bool exposed) = 0;

    virtual QRenderAspect *aspect() const = 0;
    virtual NodeManagers *nodeManagers() const = 0;
    virtual Qt3DCore::QServiceLocator *services() const = 0;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void releaseGraphicsResources() = 0;

    virtual void render(bool swapBuffers) = 0;

    virtual void cleanGraphicsResources() = 0;

    virtual bool isRunning() const = 0;

    virtual void markDirty(BackendNodeDirtySet changes, BackendNode *node) = 0;
    virtual BackendNodeDirtySet dirtyBits() = 0;
#if defined(QT_BUILD_INTERNAL)
    virtual void clearDirtyBits(BackendNodeDirtySet changes) = 0;
#endif
    virtual bool shouldRender() const = 0;
    virtual void skipNextFrame() = 0;
    virtual void jobsDone(Qt3DCore::QAspectManager *manager) = 0;

    virtual bool processMouseEvent(QObject *object, QMouseEvent *event) = 0;
    virtual bool processKeyEvent(QObject *object, QKeyEvent *event) = 0;

    virtual std::vector<Qt3DCore::QAspectJobPtr> preRenderingJobs() = 0;
    virtual std::vector<Qt3DCore::QAspectJobPtr> renderBinJobs() = 0;

    virtual void setSceneRoot(Entity *root) = 0;

    virtual Entity *sceneRoot() const = 0;
    virtual FrameGraphNode *frameGraphRoot() const = 0;

    virtual Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const = 0;

    virtual void setSettings(RenderSettings *settings) = 0;
    virtual RenderSettings *settings() const = 0;

    virtual QVariant executeCommand(const QStringList &args) = 0;

    // For QtQuick rendering (Scene3D/2D)
    virtual void setOpenGLContext(QOpenGLContext *ctx) = 0;
    virtual void setRHIContext(QRhi *ctx) = 0;
    virtual void setDefaultRHIRenderTarget(QRhiRenderTarget *defaultTarget) = 0;
    virtual void setRHICommandBuffer(QRhiCommandBuffer *commandBuffer) = 0;
    virtual void setScreen(QScreen *) {}
    virtual QScreen *screen() const { return nullptr; }
    virtual bool accessOpenGLTexture(Qt3DCore::QNodeId nodeId, QOpenGLTexture **texture, QMutex **lock, bool readonly) = 0;
    virtual QSharedPointer<RenderBackendResourceAccessor> resourceAccessor() const = 0;

    virtual void setOffscreenSurfaceHelper(OffscreenSurfaceHelper *helper) = 0;
    virtual QSurfaceFormat format() = 0;
    virtual QOpenGLContext *shareContext() const = 0;
    virtual const GraphicsApiFilterData *contextInfo() const = 0;

    // Runtime Cache for Generated Shader Graph
    bool containsGeneratedShaderGraph(const QByteArray &key) const { return m_cachedGeneratedShaderGraphes.contains(key); };
    QByteArray cachedGeneratedShaderGraph(const QByteArray &key) const { return m_cachedGeneratedShaderGraphes.value(key); };
    void insertGeneratedShaderGraph(const QByteArray &key, const QByteArray shaderCode) { m_cachedGeneratedShaderGraphes.insert(key, shaderCode); }
    void removeGeneratedShaderGraph(const QByteArray &key) { m_cachedGeneratedShaderGraphes.remove(key); };

private:
    QHash<QByteArray, QByteArray> m_cachedGeneratedShaderGraphes;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractRenderer::BackendNodeDirtySet)

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H

