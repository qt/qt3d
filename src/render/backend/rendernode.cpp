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

#include "rendernode.h"
#include "rendereraspect.h"
#include "renderer.h"
#include "rendernodesmanager.h"
#include "matrixmanager.h"

#include <transform.h>

#include <qaspectmanager.h>
#include <sphere.h>
#include <entity.h>

#include <QMatrix4x4>
#include <QString>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

// TODO: Create custom allocators for each of the matrices and
// bounding volumes to that they end up in contiguous arrays.

RenderNode::RenderNode()
    : m_renderer(Q_NULLPTR)
    , m_transform(Q_NULLPTR)
    , m_localBoundingVolume(new Qt3D::Sphere)
    , m_worldBoundingVolume(new Qt3D::Sphere)
    , m_frontEndPeer(0)
{
}

void RenderNode::setParentHandle(HRenderNode parentHandle)
{
    Q_ASSERT(m_renderer);
    m_parentHandle = parentHandle;
    RenderNode *parent = m_renderer->renderNodesManager()->data(parentHandle);
    if (parent != Q_NULLPTR && !parent->m_childrenHandles.contains(m_handle))
        parent->m_childrenHandles.append(m_handle);
}

void RenderNode::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderNode::setHandle(HRenderNode handle)
{
    m_handle = handle;
}

void RenderNode::setFrontEndPeer(Node *peer)
{
    m_frontEndPeer = peer;

    if (!m_localTransform.isNull())
        m_renderer->localMatrixManager()->release(m_localTransform);
    if (!m_worldTransform.isNull())
        m_renderer->worldMatrixManager()->release(m_worldTransform);

    if (m_frontEndPeer->asEntity()) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        arbiter->registerObserver(this, m_frontEndPeer->asEntity(), AllChanges);
        m_localTransform = m_renderer->localMatrixManager()->getOrAcquireHandle(peer->asEntity()->uuid());
        m_worldTransform = m_renderer->worldMatrixManager()->getOrAcquireHandle(peer->asEntity()->uuid());
    }
    else {
        m_localTransform = m_renderer->localMatrixManager()->acquire();
        m_worldTransform = m_renderer->worldMatrixManager()->acquire();
    }
}

void RenderNode::setTransform(Transform *transform)
{
    m_transform = transform;

    // Register for changes
    QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
    arbiter->registerObserver(this, m_transform, LocalTransform);
}

void RenderNode::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->m_type) {
    case LocalTransform: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        *localTransform() = propertyChange->m_value.value<QMatrix4x4>();
        qCDebug(RenderNodes) << Q_FUNC_INFO << "Updating transform";
        break;
    }
    case AllChanges: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        qCDebug(RenderNodes) << Q_FUNC_INFO << "ALLCHANGES RECEIVED" << propertyChange->m_value;
        break;
    }

    default:
        break;
    }
}

void RenderNode::dump() const
{
    static int depth = 0;
    QString indent(2 * depth++, QChar::fromLatin1(' '));
    qCDebug(Backend) << indent + m_frontEndPeer->objectName();
    foreach (const RenderNode *child, children())
        child->dump();
    --depth;
}

RenderNode *RenderNode::parent() const
{
    return m_renderer->renderNodesManager()->data(m_parentHandle);
}

void RenderNode::appendChildHandle(HRenderNode childHandle)
{
    if (!m_childrenHandles.contains(childHandle)) {
        m_childrenHandles.append(childHandle);
        RenderNode *child = m_renderer->renderNodesManager()->data(childHandle);
        if (child != Q_NULLPTR)
            child->m_parentHandle = m_handle;
    }
}

QVector<RenderNode *> RenderNode::children() const
{
    QVector<RenderNode *> childrenVector;
    childrenVector.reserve(m_childrenHandles.size());
    foreach (HRenderNode handle, m_childrenHandles) {
        RenderNode *child = m_renderer->renderNodesManager()->data(handle);
        if (child != Q_NULLPTR)
            childrenVector.append(child);
    }
    return childrenVector;
}

QMatrix4x4 *RenderNode::localTransform() { return m_renderer->localMatrixManager()->data(m_localTransform); }

QMatrix4x4 *RenderNode::worldTransform() { return m_renderer->worldMatrixManager()->data(m_worldTransform); }

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
