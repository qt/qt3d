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

#ifndef QT3D_RENDER_RENDERER_H
#define QT3D_RENDER_RENDERER_H

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/private/quniformvalue_p.h>
#include <Qt3DRenderer/private/handle_types_p.h>
#include <Qt3DCore/qaspectjob.h>

#include <QHash>
#include <QMatrix4x4>
#include <QObject>
#include <QTimer>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>
#include <QScopedPointer>
#include <QThreadStorage>

QT_BEGIN_NAMESPACE

class QSurface;
class QOpenGLDebugLogger;

namespace Qt3D {

class QCamera;
class QEntity;
class QMaterial;
class QShaderProgram;
class QMesh;
class QRenderPass;
class QAbstractShapeMesh;
class QFrameAllocator;
class QOpenGLFilter;
class AbstractSceneParser;

typedef QVector<QFrameAllocator *> QFrameAllocatorQueue;

namespace Render {

class RenderTextureProvider;
class RenderCameraLens;
class QGraphicsContext;
class FrameGraphNode;
class RenderMaterial;
class RenderTechnique;
class RenderShader;
class RenderEntity;
class MeshDataManager;
class MeshManager;
class RenderCommand;
class CameraManager;
class EntityManager;
class RenderQueues;
class RenderView;
class MaterialManager;
class MatrixManager;
class VAOManager;
class ShaderManager;
class TechniqueManager;
class EffectManager;
class RenderPassManager;
class RenderEffect;
class RenderRenderPass;
class TextureManager;
class TextureDataManager;
class LayerManager;
class LightManager;
class RenderThread;
class CriterionManager;
class FrameGraphManager;
class TransformManager;
class RenderStateSet;
class RenderTargetManager;
class SceneManager;
class AttachmentManager;
class SortCriterionManager;
class ParameterManager;
class ShaderDataManager;
class UBOManager;
class TextureImageManager;

class Renderer
{
public:
    explicit Renderer(QRenderAspect::RenderType type, int cachedFrames = 5);
    ~Renderer();

    void setQRenderAspect(QRenderAspect *aspect) { m_rendererAspect = aspect; }
    QRenderAspect *rendererAspect() const { return m_rendererAspect; }

    void createAllocators();
    void destroyAllocators();

    QThreadStorage<QPair<int, QFrameAllocatorQueue *> *> *tlsAllocators();

    void setFrameGraphRoot(const QNodeId &fgRoot);
    Render::FrameGraphNode *frameGraphRoot() const;

    void setSceneGraphRoot(RenderEntity *sgRoot);
    RenderEntity *renderSceneRoot() const { return m_renderSceneRoot; }

    void render();
    void doRender(int maxFrameCount = -1);

    QVector<QAspectJobPtr> createRenderBinJobs();
    QAspectJobPtr createRenderViewJob(FrameGraphNode *node, int submitOrderIndex);
    void executeCommands(const QVector<RenderCommand *> &commands);

    inline RenderQueues* renderQueues() const { return m_renderQueues; }
    inline MeshDataManager *meshDataManager() const { return m_meshDataManager; }
    inline MeshManager *meshManager() const { return m_meshManager; }
    inline CameraManager *cameraManager() const { return m_cameraManager; }
    inline EntityManager *renderNodesManager() const { return m_renderNodesManager; }
    inline MaterialManager *materialManager() const { return m_materialManager; }
    inline MatrixManager *worldMatrixManager() const { return m_worldMatrixManager; }
    inline VAOManager *vaoManager() const { return m_vaoManager; }
    inline ShaderManager *shaderManager() const { return m_shaderManager; }
    inline TechniqueManager *techniqueManager() const { return m_techniqueManager; }
    inline EffectManager *effectManager() const { return m_effectManager; }
    inline RenderPassManager *renderPassManager() const { return m_renderPassManager; }
    inline TextureManager *textureManager() const { return m_textureManager; }
    inline TextureDataManager *textureDataManager() const { return m_textureDataManager; }
    inline LayerManager *layerManager() const { return m_layerManager; }
    inline CriterionManager *criterionManager() const { return m_criterionManager; }
    inline FrameGraphManager *frameGraphManager() const { return m_frameGraphManager; }
    inline TransformManager *transformManager() const { return m_transformManager; }
    inline RenderTargetManager *renderTargetManager() const { return m_renderTargetManager; }
    inline SceneManager *sceneManager() const { return m_sceneManager; }
    inline AttachmentManager *attachmentManager() const { return m_attachmentManager; }
    inline SortCriterionManager *sortCriterionManager() const { return m_sortCriterionManager; }
    inline ParameterManager *parameterManager() const { return m_parameterManager; }
    inline ShaderDataManager *shaderDataManager() const { return m_shaderDataManager; }
    inline UBOManager *uboManager() const { return m_uboManager; }
    inline TextureImageManager *textureImageManager() const { return m_textureImageManager; }

    inline HMaterial defaultMaterialHandle() const { return m_defaultMaterialHandle; }
    inline HEffect defaultEffectHandle() const { return m_defaultEffectHandle; }
    inline HTechnique defaultTechniqueHandle() const { return m_defaultTechniqueHandle; }
    inline HRenderPass defaultRenderPassHandle() const { return m_defaultRenderPassHandle; }
    inline RenderStateSet *defaultRenderState() const { return m_defaultRenderStateSet; }

    inline QList<AbstractSceneParser *> sceneParsers() const { return m_sceneParsers; }

    QOpenGLFilter *contextInfo() const;

    inline int cachedFramesCount() const { return m_cachedFramesCount; }

    void setSurface(QSurface *s);

    void enqueueRenderView(RenderView *renderView, int submitOrder);
    void submitRenderViews(int maxFrameCount = -1);

    void initialize(QOpenGLContext *context = Q_NULLPTR);
    void shutdown();

    QFrameAllocator *currentFrameAllocator(int frameIndex);

    QMutex* mutex() { return &m_mutex; }

private:
    bool canRender() const;

    QRenderAspect *m_rendererAspect;

    // Frame graph root
    QNodeId m_frameGraphRootUuid;

    RenderEntity *m_renderSceneRoot;

    QHash<QMaterial*, RenderMaterial*> m_materialHash;
    QHash<QTechnique *, RenderTechnique*> m_techniqueHash;
    QHash<QShaderProgram*, RenderShader*> m_shaderHash;

    QMaterial* m_defaultMaterial;
    QTechnique* m_defaultTechnique;

    HMaterial m_defaultMaterialHandle;
    HEffect m_defaultEffectHandle;
    HTechnique m_defaultTechniqueHandle;
    HRenderPass m_defaultRenderPassHandle;

    // Fail safe values that we can use if a RenderCommand
    // is missing a shader
    RenderShader *m_defaultRenderShader;
    RenderStateSet *m_defaultRenderStateSet;
    QHash<QString, QString> m_defaultParameterToGLSLAttributeNames;
    QUniformPack m_defaultUniformPack;

    QScopedPointer<QGraphicsContext> m_graphicsContext;
    QSurface *m_surface;
    QScopedPointer<RenderTextureProvider> m_textureProvider;
    MeshDataManager *m_meshDataManager;
    MeshManager *m_meshManager;
    CameraManager *m_cameraManager;
    EntityManager *m_renderNodesManager;
    MaterialManager *m_materialManager;
    MatrixManager *m_worldMatrixManager;
    VAOManager *m_vaoManager;
    ShaderManager *m_shaderManager;
    TechniqueManager *m_techniqueManager;
    EffectManager *m_effectManager;
    RenderPassManager *m_renderPassManager;
    TextureManager *m_textureManager;
    TextureDataManager *m_textureDataManager;
    LayerManager *m_layerManager;
    CriterionManager *m_criterionManager;
    FrameGraphManager *m_frameGraphManager;
    TransformManager *m_transformManager;
    RenderTargetManager *m_renderTargetManager;
    SceneManager *m_sceneManager;
    AttachmentManager *m_attachmentManager;
    SortCriterionManager *m_sortCriterionManager;
    ParameterManager *m_parameterManager;
    ShaderDataManager *m_shaderDataManager;
    UBOManager *m_uboManager;
    TextureImageManager *m_textureImageManager;

    QTimer *m_frameTimer;

    RenderQueues *m_renderQueues;
    QScopedPointer<RenderThread> m_renderThread;

    void buildDefaultMaterial();
    void buildDefaultTechnique();
    void loadSceneParsers();

    QMutex m_mutex;
    QWaitCondition m_submitRenderViewsCondition;
    QWaitCondition m_waitForWindowToBeSetCondition;
    QWaitCondition m_waitForInitializationToBeCompleted;
    uint m_frameCount;
    int m_currentPreprocessingFrameIndex;

    static void createThreadLocalAllocator(void *renderer);
    static void destroyThreadLocalAllocator(void *renderer);
    QThreadStorage< QPair<int, QFrameAllocatorQueue *> *> m_tlsAllocators;

    const int m_cachedFramesCount;
    QAtomicInt m_running;

    QScopedPointer<QOpenGLDebugLogger> m_debugLogger;
    QList<AbstractSceneParser *> m_sceneParsers;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERER_H
