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

#include "renderentity.h"
#include "rendereraspect.h"
#include "renderer.h"
#include "rendernodesmanager.h"
#include "matrixmanager.h"
#include "cameramanager.h"
#include "materialmanager.h"
#include "meshmanager.h"
#include "meshdatamanager.h"
#include "qmesh.h"
#include "qabstractshapemesh.h"
#include <sphere.h>

#include <Qt3DCore/entity.h>
#include <Qt3DCore/cameralens.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/transform.h>

#include <QMatrix4x4>
#include <QString>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

// TODO: Create custom allocators for each of the matrices and
// bounding volumes to that they end up in contiguous arrays.

RenderEntity::RenderEntity()
    : m_renderer(Q_NULLPTR)
    , m_transform(Q_NULLPTR)
    , m_localBoundingVolume(new Qt3D::Sphere)
    , m_worldBoundingVolume(new Qt3D::Sphere)
    , m_frontEndPeer(0)
{
}

RenderEntity::~RenderEntity()
{
    delete m_localBoundingVolume;
    delete m_worldBoundingVolume;
}

void RenderEntity::setParentHandle(HRenderNode parentHandle)
{
    Q_ASSERT(m_renderer);
    m_parentHandle = parentHandle;
    RenderEntity *parent = m_renderer->renderNodesManager()->data(parentHandle);
    if (parent != Q_NULLPTR && !parent->m_childrenHandles.contains(m_handle))
        parent->m_childrenHandles.append(m_handle);
}

void RenderEntity::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderEntity::setHandle(HRenderNode handle)
{
    m_handle = handle;
}

void RenderEntity::setPeer(Entity *peer)
{
    if (m_frontEndPeer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();

        if (!m_localTransform.isNull())
            m_renderer->localMatrixManager()->release(m_localTransform);
        if (!m_worldTransform.isNull())
            m_renderer->worldMatrixManager()->release(m_worldTransform);

        m_frontEndPeer = peer;
        if (m_frontEndPeer) {
            arbiter->registerObserver(this, m_frontEndPeer, AllChanges);
            m_localTransform = m_renderer->localMatrixManager()->getOrAcquireHandle(peer->uuid());
            m_worldTransform = m_renderer->worldMatrixManager()->getOrAcquireHandle(peer->uuid());
        }
    }
}

void RenderEntity::setTransform(Transform *transform)
{
    if (transform != m_transform) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        // Unregister from changes of the previous transform
        if (m_transform)
            arbiter->unregisterObserver(this, m_transform);
        m_transform = transform;
        // Register for changes
        if (m_transform) {
            arbiter->registerObserver(this, m_transform, ComponentUpdated);
            *localTransform() = m_transform->matrix();
        }
    }
}

void RenderEntity::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->m_type) {

    case NodeCreated: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeCreated";
        break;
    }
    case NodeAboutToBeDeleted: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeAboutToBeDeleted";
        break;
    }
    case NodeDeleted: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeDeleted";
        break;
    }
    case ComponentUpdated: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        Node *node = propertyChange->m_subject.m_node;
        if (node == m_transform
                && propertyChange->m_propertyName == QByteArrayLiteral("matrix"))
            *localTransform() = propertyChange->m_value.value<QMatrix4x4>();
        break;
    }

    case ComponentAdded: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        Component *component = propertyChange->m_value.value<Component*>();
        if (m_transform == Q_NULLPTR && qobject_cast<Transform*>(component) != Q_NULLPTR) {
            setTransform(qobject_cast<Transform *>(component));
        }
        else if (qobject_cast<CameraLens *>(component)) {
            RenderCamera *cam = m_renderer->cameraManager()->lookupResource(m_frontEndPeer->asEntity()->uuid());
            if (cam != Q_NULLPTR)
                cam->setPeer(qobject_cast<CameraLens *>(component));
        }
        else if (qobject_cast<QAbstractMesh *>(component)) {
            RenderMesh *mesh = m_renderer->meshManager()->lookupResource(m_frontEndPeer->asEntity()->uuid());
            if (mesh != Q_NULLPTR) {
                mesh->setPeer(qobject_cast<QMesh *>(component));
            }
        }
        break;
    }

    case ComponentRemoved: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        Component *component = propertyChange->m_value.value<Component*>();
        if (component == m_transform) {
            setTransform(Q_NULLPTR);
        }
        else if (qobject_cast<CameraLens *>(component)) {
            RenderCamera *cam = m_renderer->cameraManager()->lookupResource(m_frontEndPeer->asEntity()->uuid());
            if (cam != Q_NULLPTR)
                cam->setPeer(Q_NULLPTR);
        }
        else if (qobject_cast<QAbstractMesh *>(component)) {
            RenderMesh *mesh = m_renderer->meshManager()->lookupResource(m_frontEndPeer->asEntity()->uuid());
            if (mesh != Q_NULLPTR)
                mesh->setPeer(Q_NULLPTR);
        }
        break;
    }

    default:
        break;
    }
}

void RenderEntity::dump() const
{
    static int depth = 0;
    QString indent(2 * depth++, QChar::fromLatin1(' '));
    qCDebug(Backend) << indent + m_frontEndPeer->objectName();
    foreach (const RenderEntity *child, children())
        child->dump();
    --depth;
}

RenderEntity *RenderEntity::parent() const
{
    return m_renderer->renderNodesManager()->data(m_parentHandle);
}

void RenderEntity::appendChildHandle(HRenderNode childHandle)
{
    if (!m_childrenHandles.contains(childHandle)) {
        m_childrenHandles.append(childHandle);
        RenderEntity *child = m_renderer->renderNodesManager()->data(childHandle);
        if (child != Q_NULLPTR)
            child->m_parentHandle = m_handle;
    }
}

QVector<RenderEntity *> RenderEntity::children() const
{
    QVector<RenderEntity *> childrenVector;
    childrenVector.reserve(m_childrenHandles.size());
    foreach (HRenderNode handle, m_childrenHandles) {
        RenderEntity *child = m_renderer->renderNodesManager()->data(handle);
        if (child != Q_NULLPTR)
            childrenVector.append(child);
    }
    return childrenVector;
}

QMatrix4x4 *RenderEntity::localTransform() { return m_renderer->localMatrixManager()->data(m_localTransform); }

QMatrix4x4 *RenderEntity::worldTransform() { return m_renderer->worldMatrixManager()->data(m_worldTransform); }

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
