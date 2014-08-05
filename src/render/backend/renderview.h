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
#include <Qt3DRenderer/rendercameralens.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DCore/qhandle.h>
#include <QVector>
#include <QMutex>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QRenderPass;
class QFrameAllocator;

namespace Render {

class Renderer;
class RenderCommand;
class RenderPassFilter;
class TechniqueFilter;
class ViewportNode;
class RenderMesh;
class RenderEffect;
class RenderRenderPass;
class RenderLight;

typedef QHandle<RenderMesh, 16> HMesh;
typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<RenderTechnique, 16> HTechnique;
typedef QHandle<RenderLight, 16> HLight;

// This class is kind of analogous to RenderBin but I want to avoid trampling
// on that until we get this working
class Q_AUTOTEST_EXPORT RenderView
{
public:
    RenderView();
    ~RenderView();

    static void operator delete(void *ptr);

    void setConfigFromFrameGraphLeafNode(FrameGraphNode *fgLeaf);

    // TODO: Add a way to specify a sort predicate for the RenderCommands
    void sort();

    // These pointers define the configuration that needs to be
    // set for this RenderView before it's contained RenderCommands
    // can be submitted. If a pointer is null, that pice of state
    // does not need to be changed.
    // TODO: Add RenderTarget

    void setRenderer(Renderer *renderer);
    void setAllocator(QFrameAllocator *allocator);

    void buildRenderCommands(RenderEntity *node);
    QVector<RenderCommand *> commands() const { return m_commands; }

    inline QRectF viewport() const { return m_viewport; }
    inline QColor clearColor() const { return m_clearColor; }

    // This can be removed once we're 100% sure the QFrameAllocator is death proof
    void setFrameIndex(int frameIndex) { m_frameIndex = frameIndex; }
    int frameIndex() const { return m_frameIndex; }

private:

    void computeViewport(ViewportNode *viewportNode);
    bool checkContainedWithinLayer(RenderEntity *node);

    RenderTechnique *findTechniqueForEffect(RenderEffect *effect);
    QList<RenderRenderPass *> findRenderPassesForTechnique(RenderTechnique *technique);
    void    createRenderTexture(Texture *tex);
    void setShaderAndUniforms(RenderCommand *command, RenderRenderPass *pass, QHash<QString, QVariant> &parameters);
    QHash<QString, QVariant> parametersFromMaterialEffectTechnique(RenderMaterial *material, RenderEffect *effect, RenderTechnique *technique);

    Renderer *m_renderer;
    QFrameAllocator *m_allocator;
    RenderCameraLens *m_renderCamera;
    TechniqueFilter *m_techniqueFilter;
    RenderPassFilter *m_passFilter;
    QRectF m_viewport;
    QStringList m_layers;
    QMatrix4x4 *m_viewMatrix;
    QColor m_clearColor;
    QList<QPair<HLight, QMatrix4x4> > m_lights;

    int m_frameIndex;

    // We do not use pointers to RenderNodes or Drawable's here so that the
    // render aspect is free to change the drawables on the next frame whilst
    // the render thread is submitting these commands.
    QVector<RenderCommand *> m_commands;

    typedef QHash<QString, QUniformValue* (RenderView::*)(const QMatrix4x4& model) const> standardUniformsPFuncsHash;
    static  standardUniformsPFuncsHash m_standardUniformSetters;
    static standardUniformsPFuncsHash initializeStandardUniformSetters();
    static QStringList m_standardAttributesNames;
    static QStringList initializeStandardAttributeNames();

    QUniformValue *modelMatrix(const QMatrix4x4& model) const;
    QUniformValue *viewMatrix(const QMatrix4x4&) const;
    QUniformValue *projectionMatrix(const QMatrix4x4 &) const;
    QUniformValue *modelViewMatrix(const QMatrix4x4 &model) const;
    QUniformValue *modelViewProjectionMatrix(const QMatrix4x4 &model) const;
    QUniformValue *inverseModelMatrix(const QMatrix4x4 &model) const;
    QUniformValue *inverseViewMatrix(const QMatrix4x4 &) const;
    QUniformValue *inverseProjectionMatrix(const QMatrix4x4 &) const;
    QUniformValue *inverseModelViewMatrix(const QMatrix4x4 &model) const;
    QUniformValue *inverseModelViewProjectionMatrix(const QMatrix4x4 &model) const;
    QUniformValue *modelNormalMatrix(const QMatrix4x4 &model) const;
    QUniformValue *modelViewNormalMatrix(const QMatrix4x4 &model) const;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERVIEW_H
