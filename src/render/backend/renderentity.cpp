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

#include "renderentity_p.h"
#include <Qt3DRenderer/private/cameramanager_p.h>
#include <Qt3DRenderer/private/entitymanager_p.h>
#include <Qt3DRenderer/private/layermanager_p.h>
#include <Qt3DRenderer/private/lightmanager_p.h>
#include <Qt3DRenderer/private/materialmanager_p.h>
#include <Qt3DRenderer/private/matrixmanager_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/meshmanager_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderscenebuilder_p.h>
#include <Qt3DRenderer/private/transformmanager_p.h>
#include <Qt3DRenderer/qabstractlight.h>
#include <Qt3DRenderer/qabstractshapemesh.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/renderlogging.h>
#include <Qt3DRenderer/sphere.h>

#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qtransform.h>

#include <QMatrix4x4>
#include <QString>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

// TODO: Create custom allocators for each of the matrices and
// bounding volumes to that they end up in contiguous arrays.

RenderEntity::RenderEntity()
    : m_renderer(Q_NULLPTR)
    , m_localBoundingVolume(new Qt3D::Sphere)
    , m_worldBoundingVolume(new Qt3D::Sphere)
{
}

RenderEntity::~RenderEntity()
{
    cleanup();
}

void RenderEntity::cleanup()
{
    if (m_renderer != Q_NULLPTR) {
        RenderEntity *parentEntity = parent();
        if (parentEntity != Q_NULLPTR)
            parentEntity->removeChildHandle(m_handle);
        for (int i = 0; i < m_childrenHandles.size(); i++)
            m_renderer->renderNodesManager()->release(m_childrenHandles[i]);
        m_renderer->worldMatrixManager()->release(m_worldTransform);

        // Release all component will have to perform their own release when they receive the
        // NodeDeleted/NodeAboutToBeDeleted notification
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO;

        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_frontendUuid);
    }
    delete m_localBoundingVolume;
    delete m_worldBoundingVolume;
}

void RenderEntity::setParentHandle(HEntity parentHandle)
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

void RenderEntity::setHandle(HEntity handle)
{
    m_handle = handle;
}

void RenderEntity::setPeer(QEntity *peer)
{
    QUuid peerUuid;
    if (peer != Q_NULLPTR)
        peerUuid = peer->uuid();
    if (m_frontendUuid != peerUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_worldTransform.isNull())
            m_renderer->worldMatrixManager()->release(m_worldTransform);

        if (m_frontendUuid.isNull()) {
            arbiter->unregisterObserver(this, m_frontendUuid);
            m_objectName.clear();
        }

        m_frontendUuid = peerUuid;
        if (!m_frontendUuid.isNull()) {
            arbiter->registerObserver(this, m_frontendUuid, AllChanges);
            m_objectName = peer->objectName();
            m_worldTransform = m_renderer->worldMatrixManager()->getOrAcquireHandle(m_frontendUuid);
        }
    }
}

void RenderEntity::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case ComponentAdded: {
        QComponent *component = qobject_cast<QComponent *>(propertyChange->value().value<QNodePtr>().data());
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Added" << m_objectName << component->objectName();
        addComponent(component);
        break;
    }

    case ComponentRemoved: {
        QComponent *component = qobject_cast<QComponent *>(propertyChange->value().value<QNodePtr>().data());
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Removed";
        removeComponent(component);
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
    qCDebug(Backend) << indent + m_objectName;
    foreach (const RenderEntity *child, children())
        child->dump();
    --depth;
}

RenderEntity *RenderEntity::parent() const
{
    return m_renderer->renderNodesManager()->data(m_parentHandle);
}

void RenderEntity::appendChildHandle(HEntity childHandle)
{
    if (!m_childrenHandles.contains(childHandle)) {
        m_childrenHandles.append(childHandle);
        RenderEntity *child = m_renderer->renderNodesManager()->data(childHandle);
        if (child != Q_NULLPTR)
            child->m_parentHandle = m_handle;
    }
}

void RenderEntity::removeChildHandle(HEntity childHandle)
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
    foreach (HEntity handle, m_childrenHandles) {
        RenderEntity *child = m_renderer->renderNodesManager()->data(handle);
        if (child != Q_NULLPTR)
            childrenVector.append(child);
    }
    return childrenVector;
}

QMatrix4x4 *RenderEntity::worldTransform() { return m_renderer->worldMatrixManager()->data(m_worldTransform); }

void RenderEntity::addComponent(QComponent *component)
{
    // The backend element is always created when this method is called
    // If that's not the case something has gone wrong

    if (qobject_cast<QTransform*>(component) != Q_NULLPTR)
        m_transformComponent = component->uuid();
    else if (qobject_cast<QAbstractMesh *>(component) != Q_NULLPTR)
        m_meshComponent = component->uuid();
    else if (qobject_cast<QCameraLens *>(component) != Q_NULLPTR)
        m_cameraComponent = component->uuid();
    else if (qobject_cast<QLayer *>(component) != Q_NULLPTR)
        m_layerComponent = component->uuid();
    else if (qobject_cast<QAbstractLight *>(component) != Q_NULLPTR)
        m_lightComponent = component->uuid();
    else if (qobject_cast<QMaterial *>(component) != Q_NULLPTR)
        m_materialComponent = component->uuid();
}

void RenderEntity::removeComponent(QComponent *component)
{
    if (qobject_cast<QTransform *>(component) != Q_NULLPTR)
        m_transformComponent = QUuid();
    else if (qobject_cast<QAbstractMesh *>(component) != Q_NULLPTR)
        m_meshComponent = QUuid();
    else if (qobject_cast<QCameraLens *>(component) != Q_NULLPTR)
        m_cameraComponent = QUuid();
    else if (qobject_cast<QLayer *>(component) != Q_NULLPTR)
        m_layerComponent = QUuid();
    else if (qobject_cast<QAbstractLight *>(component) != Q_NULLPTR)
        m_lightComponent = QUuid();
    else if (qobject_cast<QMaterial *>(component) != Q_NULLPTR)
        m_materialComponent = QUuid();
}

template<>
HMesh RenderEntity::componentHandle<RenderMesh>() const
{
    return m_renderer->meshManager()->lookupHandle(m_meshComponent);
}

template<>
RenderMesh *RenderEntity::renderComponent<RenderMesh>() const
{
    return m_renderer->meshManager()->lookupResource(m_meshComponent);
}

template<>
HMaterial RenderEntity::componentHandle<RenderMaterial>() const
{
    return m_renderer->materialManager()->lookupHandle(m_materialComponent);
}

template<>
RenderMaterial *RenderEntity::renderComponent<RenderMaterial>() const
{
    return m_renderer->materialManager()->lookupResource(m_materialComponent);
}

template<>
HLayer RenderEntity::componentHandle<RenderLayer>() const
{
    return m_renderer->layerManager()->lookupHandle(m_layerComponent);
}

template<>
RenderLayer *RenderEntity::renderComponent<RenderLayer>() const
{
    return m_renderer->layerManager()->lookupResource(m_layerComponent);
}

template<>
HLight RenderEntity::componentHandle<RenderLight>() const
{
    return m_renderer->lightManager()->lookupHandle(m_lightComponent);
}

template<>
RenderLight *RenderEntity::renderComponent<RenderLight>() const
{
    return m_renderer->lightManager()->lookupResource(m_lightComponent);
}

template<>
HCamera RenderEntity::componentHandle<RenderCameraLens>() const
{
    return m_renderer->cameraManager()->lookupHandle(m_cameraComponent);
}

template<>
RenderCameraLens *RenderEntity::renderComponent<RenderCameraLens>() const
{
    return m_renderer->cameraManager()->lookupResource(m_cameraComponent);
}

template<>
HTransform RenderEntity::componentHandle<RenderTransform>() const
{
    return m_renderer->transformManager()->lookupHandle(m_transformComponent);
}

template<>
RenderTransform *RenderEntity::renderComponent<RenderTransform>() const
{
    return m_renderer->transformManager()->lookupResource(m_transformComponent);
}

template<>
QUuid RenderEntity::componentUuid<RenderTransform>() const { return m_transformComponent; }

template<>
QUuid RenderEntity::componentUuid<RenderCameraLens>() const { return m_cameraComponent; }

template<>
QUuid RenderEntity::componentUuid<RenderLayer>() const { return m_layerComponent; }

template<>
QUuid RenderEntity::componentUuid<RenderMaterial>() const { return m_materialComponent; }

template<>
QUuid RenderEntity::componentUuid<RenderLight>() const { return m_lightComponent; }

template<>
QUuid RenderEntity::componentUuid<RenderMesh>() const { return m_meshComponent; }


} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
