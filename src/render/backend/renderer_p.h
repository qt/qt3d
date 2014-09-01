/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_RENDERER_H
#define QT3D_RENDER_RENDERER_H

#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/private/quniformvalue_p.h>
#include <Qt3DCore/qjob.h>
#include <Qt3DCore/qhandle.h>

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
class RendererAspect;
class QFrameAllocator;
class QOpenGLFilter;

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
class RenderSceneBuilder;
class FrameGraphManager;
class TransformManager;
class RenderStateSet;
class RenderTargetManager;
class SceneManager;

typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<RenderEffect, 16> HEffect;
typedef QHandle<RenderRenderPass, 16> HRenderPass;
typedef QHandle<RenderTechnique, 16> HTechnique;

class Renderer
{
public:
    explicit Renderer(int cachedFrames = 5);
    ~Renderer();

    void setRendererAspect(RendererAspect *aspect) { m_rendererAspect = aspect; }
    RendererAspect *rendererAspect() const { return m_rendererAspect; }

    void setFrameGraphRoot(Render::FrameGraphNode *fgRoot);
    Render::FrameGraphNode *frameGraphRoot() const;

    void setSceneGraphRoot(QEntity *sgRoot);
    QEntity *sceneGraphRoot() const;
    RenderEntity *renderSceneRoot() const { return m_renderSceneRoot; }

    void render();
    void doRender();

    QVector<QJobPtr> createRenderBinJobs();
    QJobPtr createRenderViewJob(FrameGraphNode *node, int submitOrderIndex);
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
    inline LightManager *lightManager() const { return m_lightManager; }
    inline CriterionManager *criterionManager() const { return m_criterionManager; }
    inline FrameGraphManager *frameGraphManager() const { return m_frameGraphManager; }
    inline TransformManager *transformManager() const { return m_transformManager; }
    inline RenderTargetManager *renderTargetManager() const { return m_renderTargetManager; }
    inline SceneManager *sceneManager() const { return m_sceneManager; }

    inline HMaterial defaultMaterialHandle() const { return m_defaultMaterialHandle; }
    inline HEffect defaultEffectHandle() const { return m_defaultEffectHandle; }
    inline HTechnique defaultTechniqueHandle() const { return m_defaultTechniqueHandle; }
    inline HRenderPass defaultRenderPassHandle() const { return m_defaultRenderPassHandle; }

    inline RenderSceneBuilder *renderSceneBuilder() const { return m_renderSceneBuilder; }

    QOpenGLFilter *contextInfo() const;

    inline int cachedFramesCount() const { return m_cachedFramesCount; }

    void setSurface(QSurface *s);

    void enqueueRenderView(RenderView *renderView, int submitOrder);
    void submitRenderViews();

    void initialize();

    QFrameAllocator *currentFrameAllocator(int frameIndex);

    QMutex* mutex() { return &m_mutex; }

private:
    RendererAspect *m_rendererAspect;

    // Frame graph root
    Render::FrameGraphNode *m_frameGraphRoot;

    QEntity *m_sceneGraphRoot;
    RenderEntity *m_renderSceneRoot;

    QHash<QMaterial*, RenderMaterial*> m_materialHash;
    QHash<QTechnique *, RenderTechnique*> m_techniqueHash;
    QHash<QShaderProgram*, RenderShader*> m_shaderHash;

    RenderTechnique *createTechnique(QTechnique *tech);
    RenderShader *getOrCreateShader(QShaderProgram *sp);
    RenderMaterial *getOrCreateMaterial(QMaterial *mat);

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
    LightManager *m_lightManager;
    CriterionManager *m_criterionManager;
    FrameGraphManager *m_frameGraphManager;
    TransformManager *m_transformManager;
    RenderTargetManager *m_renderTargetManager;
    SceneManager *m_sceneManager;

    QTimer *m_frameTimer;

    RenderQueues *m_renderQueues;
    QScopedPointer<RenderThread> m_renderThread;
    RenderSceneBuilder *m_renderSceneBuilder;

    void buildDefaultMaterial();
    void buildDefaultTechnique();

    QMutex m_mutex;
    QWaitCondition m_submitRenderViewsCondition;
    QWaitCondition m_waitForWindowToBeSetCondition;
    QWaitCondition m_waitForInitializationToBeCompleted;
    uint m_frameCount;
    int m_currentPreprocessingFrameIndex;

    QThreadStorage< QPair<int, QFrameAllocatorQueue *> > m_tlsAllocators;

    const int m_cachedFramesCount;
    QAtomicInt m_running;

    QScopedPointer<QOpenGLDebugLogger> m_debugLogger;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERER_H
