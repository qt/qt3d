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

#ifndef QT3D_RENDER_RENDERSCENEBUILDER_H
#define QT3D_RENDER_RENDERSCENEBUILDER_H

#include <Qt3DCore/nodevisitor.h>
#include <Qt3DCore/qhandle.h>
#include <QStack>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class FrameGraphItem;
class FrameGraph;

namespace Render {
class RenderNode;
}

typedef QHandle<Render::RenderNode, 16> HRenderNode;

namespace Render {

class Renderer;
class FrameGraphNode;

class RenderSceneBuilder : public Qt3D::NodeVisitor
{
public:
    explicit RenderSceneBuilder(Renderer *renderer);

    RenderNode *rootNode() const;
    HRenderNode rootHandle() const { return m_rootNodeHandle; }
    void        initializeFrameGraph();

protected:
    void visitNode(Qt3D::Node *node) Q_DECL_OVERRIDE;
    void visitEntity(Qt3D::Node *entity) Q_DECL_OVERRIDE;

private:
    Renderer *m_renderer;
    HRenderNode m_rootNodeHandle;
    QStack<HRenderNode> m_nodeStack;
    HRenderNode m_frameGraphEntityNode;

    Render::FrameGraphNode* buildFrameGraph(Node *node);
    Render::FrameGraphNode* backendFrameGraphNode(Node *);

    HRenderNode createRenderNode(Node *node);
    void createRenderMesh(Entity *entity);
    void createRenderCamera(Entity *entity);
    void createRenderMaterial(Entity *entity);
    void createFrameGraph(FrameGraph *frameGraph);


};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSCENEBUILDER_H
