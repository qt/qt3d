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

#ifndef QT3D_RENDER_RENDERVIEW_H
#define QT3D_RENDER_RENDERVIEW_H

#include <Qt3DRenderer/renderer.h>
#include <Qt3DRenderer/rendercamera.h>
#include <Qt3DRenderer/parameter.h>
#include <Qt3DCore/qhandle.h>
#include <QVector>
#include <QMutex>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class RenderPass; // TODO Split into front/back ends

namespace Render {

class Renderer;
class RenderCommand;
class RenderPassFilter;
class TechniqueFilter;
class ViewportNode;
class RenderMesh;
class RenderEffect;
class RenderRenderPass;

typedef QHandle<RenderMesh, 16> HMesh;
typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<RenderTechnique, 16> HTechnique;

// This class is kind of analogous to RenderBin but I want to avoid trampling
// on that until we get this working
class Q_AUTOTEST_EXPORT RenderView
{
public:
    RenderView();
    ~RenderView();

    void setConfigFromFrameGraphLeafNode(FrameGraphNode *fgLeaf);

    // TODO: Add a way to specify a sort predicate for the RenderCommands
    void sort();

    // These pointers define the configuration that needs to be
    // set for this RenderView before it's contained RenderCommands
    // can be submitted. If a pointer is null, that pice of state
    // does not need to be changed.
    // TODO: Add RenderTarget

    void setRenderer(Renderer *renderer);


    void buildRenderCommands(RenderNode *node);
    QVector<RenderCommand *> commands() const { return m_commands; }

    inline QRectF viewport() const { return m_viewport; }

private:
    void computeViewport(ViewportNode *viewportNode);

    RenderMaterial *findMaterialForMeshNode(const QUuid &entityUuid);
    RenderEffect *findEffectForMaterial(RenderMaterial *material);
    RenderTechnique *findTechniqueForEffect(RenderEffect *effect);
    QList<RenderRenderPass *> findRenderPassesForTechnique(RenderTechnique *technique);
    void    createRenderTexture(Texture *tex);
    void setShaderAndUniforms(RenderCommand *command, RenderRenderPass *pass, QHash<QString, QVariant> &parameters);
    QHash<QString, QVariant> parametersFromMaterialEffectTechnique(RenderMaterial *material, RenderEffect *effect, RenderTechnique *technique);

    Renderer *m_renderer;
    RenderCamera *m_renderCamera;
    TechniqueFilter *m_techniqueFilter;
    RenderPassFilter *m_passFilter;
    QRectF m_viewport;

    // We do not use pointers to RenderNodes or Drawable's here so that the
    // render aspect is free to change the drawables on the next frame whilst
    // the render thread is submitting these commands.
    QVector<RenderCommand *> m_commands;

    // We cannot return a QUniformValue as otherwise callying apply will result in calling QUniformApply
    // Instead of the SpecifiedUniform<T>apply
    // This is due to the fact that storing in a QHash creates a copy of QUniformValue and therefore the SpecifiedUniform<T> gets lost
    // A dedicated ResourcesManager might improve performances later on
    // We could have pre allocated/persistent rendercommands that we reuse in the RenderQueue

    typedef QHash<Parameter::StandardUniform, QUniformValue* (RenderView::*)(RenderCamera *c, const QMatrix4x4& model) const> standardUniformsPFuncsHash;
    static  standardUniformsPFuncsHash m_standardUniformSetters;
    static standardUniformsPFuncsHash initializeStandardUniformSetters();

    inline QUniformValue *modelMatrix(RenderCamera *, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>(model); }
    inline QUniformValue *viewMatrix(RenderCamera *c, const QMatrix4x4&) const { return new SpecifiedUniform<QMatrix4x4>(c->viewMatrix());}
    inline QUniformValue *projectionMatrix(RenderCamera *c, const QMatrix4x4&) const { return new SpecifiedUniform<QMatrix4x4>(c->projection()); }
    inline QUniformValue *modelViewMatrix(RenderCamera *c, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>(c->viewMatrix() * model); }
    inline QUniformValue *modelViewProjectionMatrix(RenderCamera *c, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>(c->projection() * c->viewMatrix() * model); }
    inline QUniformValue *inversedModelMatrix(RenderCamera *, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>(model.inverted()); }
    inline QUniformValue *inversedViewMatrix(RenderCamera *c, const QMatrix4x4&) const { return new SpecifiedUniform<QMatrix4x4>(c->viewMatrix().inverted()); }
    inline QUniformValue *inversedProjectionMatrix(RenderCamera *c, const QMatrix4x4&) const { return new SpecifiedUniform<QMatrix4x4>(c->projection().inverted()); }
    inline QUniformValue *inversedModelViewMatrix(RenderCamera *c, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>((c->viewMatrix() * model).inverted()); }
    inline QUniformValue *inversedModelViewProjectionMatrix(RenderCamera *c, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix4x4>((c->projection() * c->viewMatrix() * model).inverted()); }
    inline QUniformValue *modelNormalMatrix(RenderCamera *, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix3x3>(model.normalMatrix()); }
    inline QUniformValue *modelViewNormalMatrix(RenderCamera *c, const QMatrix4x4& model) const { return new SpecifiedUniform<QMatrix3x3>((c->viewMatrix() * model).normalMatrix()); }
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERVIEW_H
