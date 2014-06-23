/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DRenderer/technique.h>
#include <Qt3DRenderer/quniformvalue.h>
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

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3D {

class Camera;
class Entity;
class Material;
class ShaderProgram;
class Mesh;
class RenderPass;
class Shape;
class RendererAspect;

namespace Render {

class RenderTextureProvider;
class RenderCamera;
class QGraphicsContext;
class FrameGraphNode;
class RenderMaterial;
class RenderTechnique;
class RenderShader;
class RenderNode;
class MeshDataManager;
class MeshManager;
class RenderCommand;
class CameraManager;
class RenderNodesManager;
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

typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<RenderEffect, 16> HEffect;
typedef QHandle<RenderRenderPass, 16> HRenderPass;
typedef QHandle<RenderTechnique, 16> HTechnique;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void setRendererAspect(RendererAspect *rendererAspect) { m_rendererAspect = rendererAspect; }
    RendererAspect *rendererAspect() const { return m_rendererAspect; }

    void setFrameGraphRoot(Render::FrameGraphNode *fgRoot);
    Render::FrameGraphNode *frameGraphRoot() const;

    void setSceneGraphRoot(Qt3D::Node *sgRoot);
    Qt3D::Node *sceneGraphRoot() const;
    RenderNode *renderSceneRoot() const { return m_renderSceneRoot; }

    void render();
    void doRender();

    QVector<QJobPtr> createRenderBinJobs();
    QJobPtr createRenderViewJob(FrameGraphNode *node, int submitOrderIndex);
    void executeCommands(const QVector<RenderCommand *> commands);

    inline RenderQueues* renderQueues() const { return m_renderQueues; }
    inline MeshDataManager *meshDataManager() const { return m_meshDataManager; }
    inline MeshManager *meshManager() const { return m_meshManager; }
    inline CameraManager *cameraManager() const { return m_cameraManager; }
    inline RenderNodesManager *renderNodesManager() const { return m_renderNodesManager; }
    inline MaterialManager *materialManager() const { return m_materialManager; }
    inline MatrixManager *worldMatrixManager() const { return m_worldMatrixManager; }
    inline MatrixManager *localMatrixManager() const { return m_localMatrixManager; }
    inline VAOManager *vaoManager() const { return m_vaoManager; }
    inline ShaderManager *shaderManager() const { return m_shaderManager; }
    inline TechniqueManager *techniqueManager() const { return m_techniqueManager; }
    inline EffectManager *effectManager() const { return m_effectManager; }
    inline RenderPassManager *renderPassManager() const { return m_renderPassManager; }
    inline TextureManager *textureManager() const { return m_textureManager; }

    inline HMaterial defaultMaterialHandle() const { return m_defaultMaterialHandle; }
    inline HEffect defaultEffectHandle() const { return m_defaultEffectHandle; }
    inline HTechnique defaultTechniqueHandle() const { return m_defaultTechniqueHandle; }
    inline HRenderPass defaultRenderPassHandle() const { return m_defaultRenderPassHandle; }

    void buildMeshes(Mesh *mesh, Material *mat, const QMatrix4x4& mm);
    void setSurface(QSurface *s);

    void enqueueRenderView(RenderView *renderView, int submitOrder);
    void submitRenderViews();

    void initialize();

    QMutex* mutex() { return &m_mutex; }

private:
    RendererAspect *m_rendererAspect;

    // Frame graph root
    Render::FrameGraphNode *m_frameGraphRoot;

    Qt3D::Node *m_sceneGraphRoot;
    RenderNode *m_renderSceneRoot;

    QHash<Material*, RenderMaterial*> m_materialHash;
    QHash<Technique *, RenderTechnique*> m_techniqueHash;
    QHash<ShaderProgram*, RenderShader*> m_shaderHash;

    RenderTechnique *createTechnique(Technique *tech);
    RenderShader *getOrCreateShader(ShaderProgram *sp);
    RenderMaterial *getOrCreateMaterial(Material *mat);

    Material* m_defaultMaterial;
    Technique* m_defaultTechnique;

    HMaterial m_defaultMaterialHandle;
    HEffect m_defaultEffectHandle;
    HTechnique m_defaultTechniqueHandle;
    HRenderPass m_defaultRenderPassHandle;

    // Fail safe values that we can use if a RenderCommand
    // is missing a shader
    RenderShader *m_defaultRenderShader;
    DrawStateSet *m_defaultDrawStateSet;
    QHash<QString, QString> m_defaultParameterToGLSLAttributeNames;
    QUniformPack m_defaultUniformPack;

    QGraphicsContext* m_graphicsContext;
    QSurface *m_surface;
    RenderTextureProvider* m_textureProvider;
    MeshDataManager *m_meshDataManager;
    MeshManager *m_meshManager;
    CameraManager *m_cameraManager;
    RenderNodesManager *m_renderNodesManager;
    MaterialManager *m_materialManager;
    MatrixManager *m_worldMatrixManager;
    MatrixManager *m_localMatrixManager;
    VAOManager *m_vaoManager;
    ShaderManager *m_shaderManager;
    TechniqueManager *m_techniqueManager;
    EffectManager *m_effectManager;
    RenderPassManager *m_renderPassManager;
    TextureManager *m_textureManager;

    QTimer* m_frameTimer;

    RenderQueues* m_renderQueues;

    void buildDefaultMaterial();
    void buildDefaultTechnique();

    QMutex m_mutex;
    QWaitCondition m_submitRenderViewsCondition;
    uint m_frameCount;
    QAtomicInt m_graphicContextInitialized;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERER_H
