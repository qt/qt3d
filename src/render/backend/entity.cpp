/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "entity_p.h"
#include "entity_p_p.h"
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/qenvironmentlight.h>
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/qlevelofdetail.h>
#include <Qt3DRender/qraycaster.h>
#include <Qt3DRender/qscreenraycaster.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>
#include <Qt3DRender/private/armature_p.h>

#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/qnodecreatedchange.h>

#include <QMatrix4x4>
#include <QString>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {


EntityPrivate::EntityPrivate()
    : Qt3DCore::QBackendNodePrivate(Entity::ReadOnly)
{
}

EntityPrivate *EntityPrivate::get(Entity *node)
{
    return node->d_func();
}

void EntityPrivate::componentAdded(Qt3DCore::QNode *frontend)
{
    Q_Q(Entity);
    const auto componentIdAndType = QNodeIdTypePair(frontend->id(), QNodePrivate::findStaticMetaObject(frontend->metaObject()));
    q->addComponent(componentIdAndType);
}

void EntityPrivate::componentRemoved(Qt3DCore::QNode *frontend)
{
    Q_Q(Entity);
    q->removeComponent(frontend->id());
}


Entity::Entity()
    : BackendNode(*new EntityPrivate)
    , m_nodeManagers(nullptr)
    , m_boundingDirty(false)
    , m_treeEnabled(true)
{
}

Entity::~Entity()
{
    cleanup();
}

void Entity::cleanup()
{
    if (m_nodeManagers != nullptr) {
        m_nodeManagers->worldMatrixManager()->releaseResource(peerId());
        qCDebug(Render::RenderNodes) << Q_FUNC_INFO;

        removeFromParentChildHandles();

        for (auto &childHandle : qAsConst(m_childrenHandles)) {
            auto child = m_nodeManagers->renderNodesManager()->data(childHandle);
            // children should always exist and have this as parent
            // if they were destroyed, they would have removed themselves from our m_childrenHandles
            Q_ASSERT(child);
            Q_ASSERT(child->m_parentHandle == m_handle);
            child->m_parentHandle = {};
        }
    }

    m_worldTransform = HMatrix();
    // Release all component will have to perform their own release when they receive the
    // NodeDeleted notification
    // Clear components
    m_transformComponent = Qt3DCore::QNodeId();
    m_cameraComponent = Qt3DCore::QNodeId();
    m_materialComponent = Qt3DCore::QNodeId();
    m_geometryRendererComponent = Qt3DCore::QNodeId();
    m_objectPickerComponent = QNodeId();
    m_boundingVolumeDebugComponent = QNodeId();
    m_computeComponent = QNodeId();
    m_armatureComponent = QNodeId();
    m_childrenHandles.clear();
    m_layerComponents.clear();
    m_levelOfDetailComponents.clear();
    m_rayCasterComponents.clear();
    m_shaderDataComponents.clear();
    m_lightComponents.clear();
    m_environmentLightComponents.clear();
    m_localBoundingVolume.reset();
    m_worldBoundingVolume.reset();
    m_worldBoundingVolumeWithChildren.reset();
    m_parentHandle = {};
    m_boundingDirty = false;
    QBackendNode::setEnabled(false);

    // Ensure we rebuild caches when an Entity gets cleaned up
    if (m_renderer)
        markDirty(AbstractRenderer::AllDirty);
}

void Entity::setParentHandle(HEntity parentHandle)
{
    Q_ASSERT(m_nodeManagers);

    if (parentHandle == m_parentHandle)
        return;

    removeFromParentChildHandles();

    m_parentHandle = parentHandle;
    auto parent = m_nodeManagers->renderNodesManager()->data(parentHandle);
    if (parent != nullptr && !parent->m_childrenHandles.contains(m_handle))
        parent->m_childrenHandles.append(m_handle);
}

void Entity::setNodeManagers(NodeManagers *manager)
{
    m_nodeManagers = manager;
}

void Entity::setHandle(HEntity handle)
{
    m_handle = handle;
}

void Entity::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const Qt3DCore::QEntity *node = qobject_cast<const Qt3DCore::QEntity *>(frontEnd);
    if (!node)
        return;

    if (this->isEnabled() != node->isEnabled()) {
        markDirty(AbstractRenderer::EntityEnabledDirty);
        // We let QBackendNode::syncFromFrontEnd change the enabled property
    }

    const auto parentID = node->parentEntity() ? node->parentEntity()->id() : Qt3DCore::QNodeId();
    auto parentHandle = m_nodeManagers->renderNodesManager()->lookupHandle(parentID);

    // All entity creation is done from top-down and always during the same frame, so
    // we if we have a valid parent node, we should always be able to resolve the
    // backend parent at this time
    Q_ASSERT(!node->parentEntity() || (!parentHandle.isNull() && m_nodeManagers->renderNodesManager()->data(parentHandle)));

    if (parentHandle != m_parentHandle) {
        markDirty(AbstractRenderer::AllDirty);
    }

    setParentHandle(parentHandle);

    if (firstTime) {
        m_worldTransform = m_nodeManagers->worldMatrixManager()->getOrAcquireHandle(peerId());

        // TODO: Suboptimal -> Maybe have a Hash<QComponent, QEntityList> instead
        m_transformComponent = QNodeId();
        m_materialComponent = QNodeId();
        m_cameraComponent = QNodeId();
        m_geometryRendererComponent = QNodeId();
        m_objectPickerComponent = QNodeId();
        m_boundingVolumeDebugComponent = QNodeId();
        m_computeComponent = QNodeId();
        m_layerComponents.clear();
        m_levelOfDetailComponents.clear();
        m_rayCasterComponents.clear();
        m_shaderDataComponents.clear();
        m_lightComponents.clear();
        m_environmentLightComponents.clear();
        m_localBoundingVolume = QSharedPointer<Sphere>::create(peerId());
        m_worldBoundingVolume = QSharedPointer<Sphere>::create(peerId());
        m_worldBoundingVolumeWithChildren = QSharedPointer<Sphere>::create(peerId());

        const QComponentVector &components = node->components();
        for (QComponent *c : components) {
            const auto idAndType = QNodeIdTypePair(c->id(), QNodePrivate::findStaticMetaObject(c->metaObject()));
            addComponent(idAndType);
        }
    }

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
}

void Entity::dump() const
{
    static int depth = 0;
    QString indent(2 * depth++, QChar::fromLatin1(' '));
    qCDebug(Backend) << indent + m_objectName;
    const auto children_ = children();
    for (const Entity *child : children_)
        child->dump();
    --depth;
}

Entity *Entity::parent() const
{
    return m_nodeManagers->renderNodesManager()->data(m_parentHandle);
}

void Entity::removeFromParentChildHandles()
{
    // remove ourself from our parent's list of children.
    auto p = parent();
    if (p)
        p->removeChildHandle(m_handle);
}

QVector<Entity *> Entity::children() const
{
    QVector<Entity *> childrenVector;
    childrenVector.reserve(m_childrenHandles.size());
    for (const HEntity &handle : m_childrenHandles) {
        Entity *child = m_nodeManagers->renderNodesManager()->data(handle);
        if (child != nullptr)
            childrenVector.append(child);
    }
    return childrenVector;
}

void Entity::traverse(const std::function<void(Entity *)> &operation)
{
    operation(this);
    for (const HEntity &handle : qAsConst(m_childrenHandles)) {
        Entity *child = m_nodeManagers->renderNodesManager()->data(handle);
        if (child != nullptr)
            child->traverse(operation);
    }
}

void Entity::traverse(const std::function<void(const Entity *)> &operation) const
{
    operation(this);
    for (const HEntity &handle : m_childrenHandles) {
        const Entity *child = m_nodeManagers->renderNodesManager()->data(handle);
        if (child != nullptr)
            child->traverse(operation);
    }
}

Matrix4x4 *Entity::worldTransform()
{
    return m_nodeManagers->worldMatrixManager()->data(m_worldTransform);
}

const Matrix4x4 *Entity::worldTransform() const
{
    return m_nodeManagers->worldMatrixManager()->data(m_worldTransform);
}

void Entity::addComponent(Qt3DCore::QNodeIdTypePair idAndType)
{
    // The backend element is always created when this method is called
    // If that's not the case something has gone wrong
    const auto type = idAndType.type;
    const auto id = idAndType.id;
    qCDebug(Render::RenderNodes) << Q_FUNC_INFO << "id =" << id << type->className();
    if (type->inherits(&Qt3DCore::QTransform::staticMetaObject)) {
        m_transformComponent = id;
    } else if (type->inherits(&QCameraLens::staticMetaObject)) {
        m_cameraComponent = id;
    } else if (type->inherits(&QLayer::staticMetaObject)) {
        m_layerComponents.append(id);
    } else if (type->inherits(&QLevelOfDetail::staticMetaObject)) {
        m_levelOfDetailComponents.append(id);
    } else if (type->inherits(&QRayCaster::staticMetaObject)) {
        m_rayCasterComponents.append(id);
    } else if (type->inherits(&QScreenRayCaster::staticMetaObject)) {
        m_rayCasterComponents.append(id);
    } else if (type->inherits(&QMaterial::staticMetaObject)) {
        m_materialComponent = id;
    } else if (type->inherits(&QAbstractLight::staticMetaObject)) { // QAbstractLight subclasses QShaderData
        m_lightComponents.append(id);
    } else if (type->inherits(&QEnvironmentLight::staticMetaObject)) {
        m_environmentLightComponents.append(id);
    } else if (type->inherits(&QShaderData::staticMetaObject)) {
        m_shaderDataComponents.append(id);
    } else if (type->inherits(&QGeometryRenderer::staticMetaObject)) {
        m_geometryRendererComponent = id;
        m_boundingDirty = true;
    } else if (type->inherits(&QObjectPicker::staticMetaObject)) {
        m_objectPickerComponent = id;
//    } else if (type->inherits(&QBoundingVolumeDebug::staticMetaObject)) {
//        m_boundingVolumeDebugComponent = id;
    } else if (type->inherits(&QComputeCommand::staticMetaObject)) {
        m_computeComponent = id;
    } else if (type->inherits(&QArmature::staticMetaObject)) {
        m_armatureComponent = id;
    }
    markDirty(AbstractRenderer::AllDirty);
}

void Entity::removeComponent(Qt3DCore::QNodeId nodeId)
{
    if (m_transformComponent == nodeId) {
        m_transformComponent = QNodeId();
    } else if (m_cameraComponent == nodeId) {
        m_cameraComponent = QNodeId();
    } else if (m_layerComponents.contains(nodeId)) {
        m_layerComponents.removeAll(nodeId);
    } else if (m_levelOfDetailComponents.contains(nodeId)) {
        m_levelOfDetailComponents.removeAll(nodeId);
    } else if (m_rayCasterComponents.contains(nodeId)) {
        m_rayCasterComponents.removeAll(nodeId);
    } else if (m_materialComponent == nodeId) {
        m_materialComponent = QNodeId();
    } else if (m_shaderDataComponents.contains(nodeId)) {
        m_shaderDataComponents.removeAll(nodeId);
    } else if (m_geometryRendererComponent == nodeId) {
        m_geometryRendererComponent = QNodeId();
        m_boundingDirty = true;
    } else if (m_objectPickerComponent == nodeId) {
        m_objectPickerComponent = QNodeId();
//    } else if (m_boundingVolumeDebugComponent == nodeId) {
//        m_boundingVolumeDebugComponent = QNodeId();
    } else if (m_lightComponents.contains(nodeId)) {
        m_lightComponents.removeAll(nodeId);
    } else if (m_environmentLightComponents.contains(nodeId)) {
        m_environmentLightComponents.removeAll(nodeId);
    } else if (m_computeComponent == nodeId) {
        m_computeComponent = QNodeId();
    } else if (m_armatureComponent == nodeId) {
        m_armatureComponent = QNodeId();
    }
    markDirty(AbstractRenderer::AllDirty);
}

bool Entity::isBoundingVolumeDirty() const
{
    return m_boundingDirty;
}

void Entity::unsetBoundingVolumeDirty()
{
    m_boundingDirty = false;
}

void Entity::addRecursiveLayerId(const QNodeId layerId)
{
    if (!m_recursiveLayerComponents.contains(layerId) && !m_layerComponents.contains(layerId))
        m_recursiveLayerComponents.push_back(layerId);
}

void Entity::removeRecursiveLayerId(const QNodeId layerId)
{
    m_recursiveLayerComponents.removeOne(layerId);
}

ENTITY_COMPONENT_TEMPLATE_IMPL(Material, HMaterial, MaterialManager, m_materialComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(CameraLens, HCamera, CameraManager, m_cameraComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(Transform, HTransform, TransformManager, m_transformComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(GeometryRenderer, HGeometryRenderer, GeometryRendererManager, m_geometryRendererComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(ObjectPicker, HObjectPicker, ObjectPickerManager, m_objectPickerComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(ComputeCommand, HComputeCommand, ComputeCommandManager, m_computeComponent)
ENTITY_COMPONENT_TEMPLATE_IMPL(Armature, HArmature, ArmatureManager, m_armatureComponent)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(Layer, HLayer, LayerManager, m_layerComponents)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(LevelOfDetail, HLevelOfDetail, LevelOfDetailManager, m_levelOfDetailComponents)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(RayCaster, HRayCaster, RayCasterManager, m_rayCasterComponents)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(ShaderData, HShaderData, ShaderDataManager, m_shaderDataComponents)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(Light, HLight, LightManager, m_lightComponents)
ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(EnvironmentLight, HEnvironmentLight, EnvironmentLightManager, m_environmentLightComponents)

RenderEntityFunctor::RenderEntityFunctor(AbstractRenderer *renderer, NodeManagers *manager)
    : m_nodeManagers(manager)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderEntityFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    HEntity renderNodeHandle = m_nodeManagers->renderNodesManager()->getOrAcquireHandle(change->subjectId());
    Entity *entity = m_nodeManagers->renderNodesManager()->data(renderNodeHandle);
    entity->setNodeManagers(m_nodeManagers);
    entity->setHandle(renderNodeHandle);
    entity->setRenderer(m_renderer);
    return entity;
}

Qt3DCore::QBackendNode *RenderEntityFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_nodeManagers->renderNodesManager()->lookupResource(id);
}

void RenderEntityFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_nodeManagers->renderNodesManager()->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
