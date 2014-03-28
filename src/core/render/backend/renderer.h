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

#include <technique.h>

#include <QHash>
#include <QMatrix4x4>
#include <QObject>
#include <QTimer>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class QSurface;

namespace Qt3D {

class Camera;
class Entity;
class Material;
class ShaderProgram;
class Mesh;
class RenderPass;
class RenderTextureProvider;
class Shape;
class RendererAspect;

namespace Render {

class RenderCamera;
class QGraphicsContext;
class Drawable;
class FrameGraphNode;
class RenderBin;
class RenderMaterial;
class RenderTechnique;
class RenderShader;
class RenderNode;
class MeshManager;

class Renderer : public QObject
{
    Q_OBJECT
public:
    Renderer();
    ~Renderer();

    void setRendererAspect(RendererAspect *rendererAspect) { m_rendererAspect = rendererAspect; }
    RendererAspect *rendererAspect() const { return m_rendererAspect; }

    void setFrameGraphRoot(FrameGraphNode *fgRoot);
    FrameGraphNode *frameGraphRoot() const;

    void setSceneObject(Qt3D::Node *obj);
    void setSceneGraphRoot(Qt3D::Node *sgRoot);
    Qt3D::Node *sceneGraphRoot() const;

    RenderNode *renderSceneRoot() const { return m_renderSceneRoot; }

    void setCamera(Camera* cam);

    void render();
    void doRender();

    MeshManager *meshManager() const { return m_meshManager; }

    // temporary!

    RenderTechnique* techniqueForMaterial(Material* mat);

    void buildMeshes(Mesh *mesh, Material *mat, const QMatrix4x4& mm);
    void buildShape(Shape *shape, Material *mat, const QMatrix4x4 &mm);

    void foundCamera(Camera* cam, const QMatrix4x4& mm);

    Q_INVOKABLE void setSurface(QSurface *s);

    void setDefaultTechnique(Technique* t);
    void setDefaultMaterial(Material* mat);

protected:
    Q_INVOKABLE void initialize();

private slots:
    void onFrame();

private:
    RendererAspect *m_rendererAspect;

    // Frame graph root
    FrameGraphNode *m_frameGraphRoot;

    Qt3D::Node *m_sceneGraphRoot;
    RenderNode *m_renderSceneRoot;

    Camera *m_camera;
    RenderCamera* m_renderCamera;

    QHash<Material*, RenderMaterial*> m_materialHash;
    QHash<Technique *, RenderTechnique*> m_techniqueHash;
    QHash<ShaderProgram*, RenderShader*> m_shaderHash;

    RenderBin *getOrCreateBinForPass(Technique *t, RenderPass *p);

    RenderTechnique *createTechnique(Technique *tech);
    RenderShader *getOrCreateShader(ShaderProgram *sp);
    RenderMaterial *getOrCreateMaterial(Material *mat);

    Material* m_defaultMaterial;
    Technique* m_defaultTechnique;

    QGraphicsContext* m_graphicsContext;
    RenderBin* m_temporaryAllBin;
    RenderTextureProvider* m_textureProvider;
    MeshManager *m_meshManager;

    QTimer* m_frameTimer;

    /// list of drawables to be initialized next frame
    QList<Drawable*> m_initList;

    void buildDefaultMaterial();
    void buildDefaultTechnique();
};

} // namespace Render
} // namespace Qt3D

#endif // QT3D_RENDER_RENDERER_H
