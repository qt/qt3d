/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderentity_p.h"
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/qabstractlight.h>
#include <Qt3DRenderer/qlayer.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/sphere.h>
#include <Qt3DRenderer/qshaderdata.h>
#include <Qt3DRenderer/qgeometryrenderer.h>
#include <Qt3DRenderer/private/geometryrenderermanager_p.h>

#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qtransform.h>

#include <QMatrix4x4>
#include <QString>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderEntity::RenderEntity()
    : QBackendNode()
    , m_renderer(Q_NULLPTR)
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
        // We need to release using peerUuid otherwise the handle will be cleared
        // but would still remain in the Id to Handle table
        m_renderer->worldMatrixManager()->releaseResource(peerUuid());
        m_worldTransform = HMatrix();

        // Release all component will have to perform their own release when they receive the
        // NodeDeleted/NodeAboutToBeDeleted notification
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO;

        // Clear components
        m_transformComponent = QNodeId();
        m_cameraComponent = QNodeId();
        m_materialComponent = QNodeId();
        m_geometryRendererComponent = QNodeId();
        m_layerComponents.clear();
        m_shaderDataComponents.clear();
    }
    delete m_localBoundingVolume;
    delete m_worldBoundingVolume;
    m_localBoundingVolume = Q_NULLPTR;
    m_worldBoundingVolume = Q_NULLPTR;


}

void RenderEntity::setParentHandle(HEntity parentHandle)
{
    Q_ASSERT(m_renderer);
    // Remove ourselves from previous parent children list
    RenderEntity *parent = m_renderer->renderNodesManager()->data(parentHandle);
    if (parent != Q_NULLPTR && parent->m_childrenHandles.contains(m_handle))
        parent->m_childrenHandles.remove(m_handle);
    m_parentHandle = parentHandle;
    parent = m_renderer->renderNodesManager()->data(parentHandle);
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

void RenderEntity::updateFromPeer(QNode *peer)
{
    QEntity *entity = static_cast<QEntity *>(peer);
    const QNodeId parentEntityId = entity->parentEntityId();

    m_objectName = peer->objectName();
    m_worldTransform = m_renderer->worldMatrixManager()->getOrAcquireHandle(peerUuid());

    // TO DO: Suboptimal -> Maybe have a Hash<QComponent, QEntityList> instead
    m_transformComponent = QNodeId();
    m_materialComponent = QNodeId();
    m_cameraComponent = QNodeId();
    m_geometryRendererComponent = QNodeId();
    m_layerComponents.clear();
    m_shaderDataComponents.clear();

    Q_FOREACH (QComponent *comp, entity->components())
        addComponent(comp);

    if (!parentEntityId.isNull()) {
        setParentHandle(m_renderer->renderNodesManager()->lookupHandle(parentEntityId));
    } else {
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "No parent entity found for Entity" << peerUuid();
    }
}

void RenderEntity::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case ComponentAdded: {
        QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
        QComponent *component = qobject_cast<QComponent *>(nodePtr.data());
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Added" << m_objectName << component->objectName();
        addComponent(component);
        break;
    }

    case ComponentRemoved: {
        QNodeId nodeId = propertyChange->value().value<QNodeId>();
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "Component Removed";
        removeComponent(nodeId);
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

QMatrix4x4 *RenderEntity::worldTransform()
{
    return m_renderer->worldMatrixManager()->data(m_worldTransform);
}

const QMatrix4x4 *RenderEntity::worldTransform() const
{
    return m_renderer->worldMatrixManager()->data(m_worldTransform);
}

void RenderEntity::addComponent(QComponent *component)
{
    // The backend element is always created when this method is called
    // If that's not the case something has gone wrong

    if (qobject_cast<QTransform*>(component) != Q_NULLPTR)
        m_transformComponent = component->id();
    else if (qobject_cast<QCameraLens *>(component) != Q_NULLPTR)
        m_cameraComponent = component->id();
    else if (qobject_cast<QLayer *>(component) != Q_NULLPTR)
        m_layerComponents.append(component->id());
    else if (qobject_cast<QMaterial *>(component) != Q_NULLPTR)
        m_materialComponent = component->id();
    else if (qobject_cast<QShaderData *>(component) != Q_NULLPTR)
        m_shaderDataComponents.append(component->id());
    else if (qobject_cast<QGeometryRenderer *>(component) != Q_NULLPTR)
        m_geometryRendererComponent = component->id();
}

void RenderEntity::removeComponent(const QNodeId &nodeId)
{
    if (m_transformComponent == nodeId)
        m_transformComponent = QNodeId();
    else if (m_cameraComponent == nodeId)
        m_cameraComponent = QNodeId();
    else if (m_layerComponents.contains(nodeId))
        m_layerComponents.removeAll(nodeId);
    else if (m_materialComponent == nodeId)
        m_materialComponent = QNodeId();
    else if (m_shaderDataComponents.contains(nodeId))
        m_shaderDataComponents.removeAll(nodeId);
    else if (m_geometryRendererComponent == nodeId)
        m_geometryRendererComponent = QNodeId();
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
HGeometryRenderer RenderEntity::componentHandle<RenderGeometryRenderer>() const
{
    return m_renderer->geometryRendererManager()->lookupHandle(m_geometryRendererComponent);
}

template<>
RenderGeometryRenderer *RenderEntity::renderComponent<RenderGeometryRenderer>() const
{
    return m_renderer->geometryRendererManager()->lookupResource(m_geometryRendererComponent);
}

template<>
QNodeId RenderEntity::componentUuid<RenderTransform>() const { return m_transformComponent; }

template<>
QNodeId RenderEntity::componentUuid<RenderCameraLens>() const { return m_cameraComponent; }

template<>
QNodeId RenderEntity::componentUuid<RenderMaterial>() const { return m_materialComponent; }

template<>
QList<HLayer> RenderEntity::componentsHandle<RenderLayer>() const
{
    QList<HLayer> layerHandles;
    Q_FOREACH (const QNodeId &id, m_layerComponents)
        layerHandles.append(m_renderer->layerManager()->lookupHandle(id));
    return layerHandles;
}

template<>
QList<RenderLayer *> RenderEntity::renderComponents<RenderLayer>() const
{
    QList<RenderLayer *> layers;
    Q_FOREACH (const QNodeId &id, m_layerComponents)
        layers.append(m_renderer->layerManager()->lookupResource(id));
    return layers;
}

template<>
QList<QNodeId> RenderEntity::componentsUuid<RenderLayer>() const { return m_layerComponents; }

template<>
QList<HShaderData> RenderEntity::componentsHandle<RenderShaderData>() const
{
    QList<HShaderData> shaderDataHandles;
    Q_FOREACH (const QNodeId &id, m_shaderDataComponents)
        shaderDataHandles.append(m_renderer->shaderDataManager()->lookupHandle(id));
    return shaderDataHandles;
}

template<>
QList<RenderShaderData *> RenderEntity::renderComponents<RenderShaderData>() const
{
    QList<RenderShaderData *> shaderDatas;
    Q_FOREACH (const QNodeId &id, m_shaderDataComponents)
        shaderDatas.append(m_renderer->shaderDataManager()->lookupResource(id));
    return shaderDatas;
}

template<>
QList<QNodeId> RenderEntity::componentsUuid<RenderShaderData>() const { return m_shaderDataComponents; }

template<>
QNodeId RenderEntity::componentUuid<RenderGeometryRenderer>() const { return m_geometryRendererComponent; }


RenderEntityFunctor::RenderEntityFunctor(Renderer *renderer)
    : m_renderer(renderer)
{
}

QBackendNode *RenderEntityFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    HEntity renderNodeHandle = m_renderer->renderNodesManager()->getOrAcquireHandle(frontend->id());
    RenderEntity *entity = m_renderer->renderNodesManager()->data(renderNodeHandle);
    entity->setFactory(factory);
    entity->setRenderer(m_renderer);
    entity->setHandle(renderNodeHandle);
    entity->setPeer(frontend);
    return entity;
}

QBackendNode *RenderEntityFunctor::get(const QNodeId &id) const
{
    return m_renderer->renderNodesManager()->lookupResource(id);
}

void RenderEntityFunctor::destroy(const QNodeId &id) const
{
    m_renderer->renderNodesManager()->releaseResource(id);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
