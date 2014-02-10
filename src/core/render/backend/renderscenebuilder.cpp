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

#include "renderscenebuilder.h"

#include "meshmanager.h"
#include "renderer.h"
#include "rendernode.h"

#include <camera.h>
#include <material.h>
#include <mesh.h>
#include <transform.h>

#include <entity.h>

namespace Qt3D {
namespace Render {

RenderSceneBuilder::RenderSceneBuilder(Renderer *renderer)
    : Qt3D::NodeVisitor()
    , m_renderer(renderer)
    , m_rootNode(0)
{
}

void RenderSceneBuilder::visitNode(Qt3D::Node *node)
{
    if (!m_rootNode) {
        m_rootNode = new RenderNode(m_renderer->rendererAspect());
        m_rootNode->m_frontEndPeer = node;
        m_nodeStack.push(m_rootNode);
    }

    Qt3D::NodeVisitor::visitNode(node);
}

void RenderSceneBuilder::visitEntity(Qt3D::Entity *entity)
{
    // Create a RenderNode corresponding to the Entity. Most data will
    // be calculated later by jobs
    RenderNode *renderNode = new RenderNode(m_renderer->rendererAspect(), m_nodeStack.top());
    renderNode->m_frontEndPeer = entity;
    *(renderNode->m_localTransform) = entity->matrix();
    m_nodeStack.push(renderNode);

    // Look for a transform component
    QList<Transform *> transforms = entity->componentsOfType<Transform>();
    if (!transforms.isEmpty())
        renderNode->setPeer(transforms.first());

//    QList<Material *> materials = entity->componentsOfType<Material>();
//    Material *material = 0;
//    if (!materials.isEmpty())
//        material = materials.first();

    // We'll update matrices in a job later. In fact should the matrix be decoupled from the mesh?
    foreach (Mesh *mesh, entity->componentsOfType<Mesh>()) {
        m_renderer->meshManager()->addMesh(mesh);
        //m_renderer->buildMeshes(mesh, material, QMatrix4x4());
    }

    //foreach (Camera *camera, entity->componentsOfType<Camera>())
    //    m_renderer->foundCamera(camera, entity->sceneMatrix());

    NodeVisitor::visitEntity(entity);

    // Coming back up the tree
    m_nodeStack.pop();
}

} // namespace Render
} // namespace Qt3D
