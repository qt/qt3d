/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RENDERER_H
#define QT3DRENDER_RENDER_RENDERER_H

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
#include <Qt3DRender/private/quniformvalue_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>

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
#include <QThreadStorage>

QT_BEGIN_NAMESPACE

class QSurface;
class QOpenGLDebugLogger;
class QMouseEvent;

namespace Qt3DCore {
class QEntity;
class QFrameAllocator;
class QEventFilterService;
}

namespace Qt3DRender {

class QCamera;
class QMaterial;
class QShaderProgram;
class QMesh;
class QRenderPass;
class QAbstractShapeMesh;
class QGraphicsApiFilter;
class QAbstractSceneParser;

namespace Render {

class CameraLens;
class GraphicsContext;
class FrameGraphNode;
class Material;
class Technique;
class Shader;
class Entity;
class RenderCommand;
class RenderQueue;
class RenderView;
class Effect;
class RenderPass;
class RenderThread;
class RenderStateSet;
class VSyncFrameAdvanceService;
class PickEventFilter;
class NodeManagers;

class QT3DRENDERSHARED_PRIVATE_EXPORT Renderer : public AbstractRenderer
{
public:
    explicit Renderer(QRenderAspect::RenderType type);
    ~Renderer();

    API api() const Q_DECL_OVERRIDE { return AbstractRenderer::OpenGL; }

    qint64 time() const Q_DECL_OVERRIDE;
    void setTime(qint64 time) Q_DECL_OVERRIDE;

    void setSurface(QSurface *s) Q_DECL_OVERRIDE;
    void setNodeManagers(NodeManagers *managers) Q_DECL_OVERRIDE { m_nodesManager = managers; }
    void setServices(Qt3DCore::QServiceLocator *services) Q_DECL_OVERRIDE { m_services = services; }
    void setSurfaceExposed(bool exposed) Q_DECL_OVERRIDE;

    QSurface *surface() const Q_DECL_OVERRIDE { return m_surface; }
    NodeManagers *nodeManagers() const Q_DECL_OVERRIDE;
    Qt3DCore::QServiceLocator *services() const Q_DECL_OVERRIDE { return m_services; }

    void initialize() Q_DECL_OVERRIDE;
    void shutdown() Q_DECL_OVERRIDE;
    void createAllocators(Qt3DCore::QAbstractAspectJobManager *jobManager) Q_DECL_OVERRIDE;
    void destroyAllocators(Qt3DCore::QAbstractAspectJobManager *jobManager) Q_DECL_OVERRIDE;

    void render() Q_DECL_OVERRIDE;
    void doRender() Q_DECL_OVERRIDE;

    bool isRunning() const Q_DECL_OVERRIDE { return m_running.load(); }

    void setSceneRoot(Qt3DCore::QBackendNodeFactory *factory, Entity *sgRoot) Q_DECL_OVERRIDE;
    Entity *sceneRoot() const Q_DECL_OVERRIDE { return m_renderSceneRoot; }

    void setFrameGraphRoot(const Qt3DCore::QNodeId fgRootId) Q_DECL_OVERRIDE;
    FrameGraphNode *frameGraphRoot() const Q_DECL_OVERRIDE;

    QVector<Qt3DCore::QAspectJobPtr> renderBinJobs() Q_DECL_OVERRIDE;
    Qt3DCore::QAspectJobPtr pickBoundingVolumeJob() Q_DECL_OVERRIDE;

    Qt3DCore::QAspectJobPtr createRenderViewJob(FrameGraphNode *node, int submitOrderIndex);

    Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const Q_DECL_OVERRIDE;

    void registerEventFilter(Qt3DCore::QEventFilterService *service) Q_DECL_OVERRIDE;

    void executeCommands(const QVector<RenderCommand *> &commands);
    Attribute *updateBuffersAndAttributes(Geometry *geometry, RenderCommand *command, GLsizei &count, bool forceUpdate);

    void setOpenGLContext(QOpenGLContext *context);
    QGraphicsApiFilter *contextInfo() const;

    void addAllocator(Qt3DCore::QFrameAllocator *allocator);

    Qt3DCore::QFrameAllocator *currentFrameAllocator();
    QThreadStorage<Qt3DCore::QFrameAllocator *> *tlsAllocators();

    inline HMaterial defaultMaterialHandle() const { return m_defaultMaterialHandle; }
    inline HEffect defaultEffectHandle() const { return m_defaultEffectHandle; }
    inline HTechnique defaultTechniqueHandle() const { return m_defaultTechniqueHandle; }
    inline HRenderPass defaultRenderPassHandle() const { return m_defaultRenderPassHandle; }
    inline RenderStateSet *defaultRenderState() const { return m_defaultRenderStateSet; }


    QList<QMouseEvent> pendingPickingEvents() const;


    void enqueueRenderView(RenderView *renderView, int submitOrder);
    bool submitRenderViews();

    QMutex* mutex() { return &m_mutex; }


#ifdef QT3D_RENDER_UNIT_TESTS
public:
#else
private:
#endif
    bool canRender() const;

    Qt3DCore::QServiceLocator *m_services;
    NodeManagers *m_nodesManager;

    // Frame graph root
    Qt3DCore::QNodeId m_frameGraphRootUuid;

    Entity *m_renderSceneRoot;

    QHash<QMaterial*, Material*> m_materialHash;
    QHash<QTechnique *, Technique*> m_techniqueHash;
    QHash<QShaderProgram*, Shader*> m_shaderHash;

    QMaterial* m_defaultMaterial;
    QTechnique* m_defaultTechnique;

    HMaterial m_defaultMaterialHandle;
    HEffect m_defaultEffectHandle;
    HTechnique m_defaultTechniqueHandle;
    HRenderPass m_defaultRenderPassHandle;

    // Fail safe values that we can use if a RenderCommand
    // is missing a shader
    Shader *m_defaultRenderShader;
    RenderStateSet *m_defaultRenderStateSet;
    QHash<QString, QString> m_defaultParameterToGLSLAttributeNames;
    QUniformPack m_defaultUniformPack;

    QScopedPointer<GraphicsContext> m_graphicsContext;
    QSurface *m_surface;


    RenderQueue *m_renderQueue;
    QScopedPointer<RenderThread> m_renderThread;
    QScopedPointer<VSyncFrameAdvanceService> m_vsyncFrameAdvanceService;

    void buildDefaultMaterial();
    void buildDefaultTechnique();

    QMutex m_mutex;
    QSemaphore m_submitRenderViewsSemaphore;
    QWaitCondition m_waitForWindowToBeSetCondition;
    QWaitCondition m_waitForInitializationToBeCompleted;

    static void createThreadLocalAllocator(void *renderer);
    static void destroyThreadLocalAllocator(void *renderer);
    QThreadStorage<Qt3DCore::QFrameAllocator *> m_tlsAllocators;

    QAtomicInt m_running;

    QScopedPointer<QOpenGLDebugLogger> m_debugLogger;
    QScopedPointer<PickEventFilter> m_pickEventFilter;
    QVector<Qt3DCore::QFrameAllocator *> m_allocators;

    QVector<Attribute *> m_dirtyAttributes;
    QVector<Geometry *> m_dirtyGeometry;
    QAtomicInt m_exposed;
    QOpenGLContext *m_glContext;
    PickBoundingVolumeJobPtr m_pickBoundingVolumeJob;

    qint64 m_time;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERER_H
