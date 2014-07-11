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
#include "entitymanager.h"
#include "matrixmanager.h"
#include "cameramanager.h"
#include "materialmanager.h"
#include "lightmanager.h"
#include "layermanager.h"
#include "meshmanager.h"
#include "meshdatamanager.h"
#include "qmesh.h"
#include "qabstractshapemesh.h"
#include "renderscenebuilder.h"
#include "qmaterial.h"
#include <sphere.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qabstractlight.h>

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
    if (m_renderer != Q_NULLPTR) {
        RenderEntity *parentEntity = parent();
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_frontEndPeer);
        if (parentEntity != Q_NULLPTR)
            parentEntity->removeChildHandle(m_handle);
        for (int i = 0; i < m_childrenHandles.size(); i++)
            m_renderer->renderNodesManager()->release(m_childrenHandles[i]);
        m_renderer->localMatrixManager()->release(m_localTransform);
        m_renderer->worldMatrixManager()->release(m_worldTransform);
    }
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

void RenderEntity::setPeer(QEntity *peer)
{
    if (m_frontEndPeer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_localTransform.isNull())
            m_renderer->localMatrixManager()->release(m_localTransform);
        if (!m_worldTransform.isNull())
            m_renderer->worldMatrixManager()->release(m_worldTransform);

        if (m_frontEndPeer) {
            arbiter->unregisterObserver(this, m_frontEndPeer);
            m_frontendUuid = QUuid();
        }

        m_frontEndPeer = peer;
        if (m_frontEndPeer) {
            arbiter->registerObserver(this, m_frontEndPeer, AllChanges);
            m_frontendUuid = peer->uuid();
            m_localTransform = m_renderer->localMatrixManager()->getOrAcquireHandle(peer->uuid());
            m_worldTransform = m_renderer->worldMatrixManager()->getOrAcquireHandle(peer->uuid());
        }
    }
}

void RenderEntity::setTransform(QTransform *transform)
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
    // TO DO: There must be a way to have this code share some part of RenderSceneBuilder
    // Try to see what's the best way to do so

    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case NodeCreated: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeCreated";
        QEntity *entity = propertyChange->value().value<QEntity *>();
        if (entity != Q_NULLPTR) {
            HRenderNode renderNodeHandle;
            renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(entity->uuid());
            RenderEntity *renderNode = m_renderer->renderNodesManager()->data(renderNodeHandle);
            renderNode->setHandle(renderNodeHandle);
            renderNode->setRenderer(m_renderer);
            renderNode->setPeer(entity);
            renderNode->setParentHandle(m_handle);
        }
        break;
    }
    case NodeAboutToBeDeleted: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeAboutToBeDeleted";
        QUuid entityId = propertyChange->value().value<QUuid>();
        m_renderer->renderNodesManager()->releaseRenderNode(entityId);
        // We also need to cleanup all Components and other resources associated with thise entity;
        break;
    }
    case NodeDeleted: {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "NodeDeleted";
        break;
    }
    case ComponentUpdated: {
        QNode *node = propertyChange->subject().m_node;
        if (node == m_transform
                && propertyChange->propertyName() == QByteArrayLiteral("matrix"))
            *localTransform() = propertyChange->value().value<QMatrix4x4>();
        break;
    }

    case ComponentAdded: {
        QComponent *component = propertyChange->value().value<QComponent*>();
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Added" << m_frontEndPeer->objectName() << component->objectName();
        if (m_transform == Q_NULLPTR && qobject_cast<QTransform*>(component) != Q_NULLPTR) {
            setTransform(qobject_cast<QTransform *>(component));
        }
        else if (qobject_cast<QAbstractMesh *>(component))
            RenderEntity::createRenderComponent<QAbstractMesh, RenderMesh, MeshManager>(qobject_cast<QAbstractMesh *>(component), m_renderer->meshManager());
        else if (qobject_cast<QCameraLens *>(component))
            RenderEntity::createRenderComponent<QCameraLens, RenderCamera, CameraManager>(qobject_cast<QCameraLens *>(component), m_renderer->cameraManager());
        else if (qobject_cast<QLayer *>(component))
            RenderEntity::createRenderComponent<QLayer, RenderLayer, LayerManager>(qobject_cast<QLayer *>(component), m_renderer->layerManager());
        else if (qobject_cast<QAbstractLight *>(component))
            RenderEntity::createRenderComponent<QAbstractLight, RenderLight, LightManager>(qobject_cast<QAbstractLight *>(component), m_renderer->lightManager());
        else if (qobject_cast<QMaterial *>(component))
            RenderEntity::createRenderComponent<QMaterial, RenderMaterial, MaterialManager>(qobject_cast<QMaterial *>(component), m_renderer->materialManager());
        break;
    }

    case ComponentRemoved: {
        QComponent *component = propertyChange->value().value<QComponent*>();
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Removed" << component->objectName();
        if (component == m_transform) {
            setTransform(Q_NULLPTR);
        }
        else if (qobject_cast<QCameraLens *>(component)) {
            m_renderer->cameraManager()->releaseResource(m_frontendUuid);
        }
        else if (qobject_cast<QAbstractMesh *>(component)) {
            qCDebug(Render::RenderNodes) << Q_FUNC_INFO << " Mesh Component Removed " << component << component->objectName();
            m_renderer->meshManager()->releaseRenderMesh(m_frontendUuid);
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

void RenderEntity::removeChildHandle(HRenderNode childHandle)
{
    // TO DO : Check if a QList here wouldn't be more performant
    if (m_childrenHandles.contains(childHandle)) {
        m_childrenHandles.removeAt(m_childrenHandles.indexOf(childHandle));
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
