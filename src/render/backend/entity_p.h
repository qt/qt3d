// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ENTITY_H
#define QT3DRENDER_RENDER_ENTITY_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qhandle_p.h>
#include <QList>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

namespace Qt3DCore {
class QNode;
class QEntity;
class QComponent;
}

namespace Qt3DRender {

class QRenderAspect;

namespace Render {

class Sphere;
class Renderer;
class NodeManagers;
class EntityPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Entity : public BackendNode
{
public:
    Entity();
    ~Entity();
    void cleanup();

    void setParentHandle(HEntity parentHandle);
    void setNodeManagers(NodeManagers *manager);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void dump() const;

    void  setHandle(HEntity handle);
    HEntity handle() const { return m_handle; }
    Entity *parent() const;
    HEntity parentHandle() const { return m_parentHandle; }

    void removeFromParentChildHandles();
    void appendChildHandle(HEntity childHandle);
    void removeChildHandle(HEntity childHandle) { m_childrenHandles.removeOne(childHandle); }
    const QList<HEntity> &childrenHandles() const { return m_childrenHandles; }
    QList<Entity *> children() const;
    bool hasChildren() const { return !m_childrenHandles.empty(); }
    void traverse(const std::function<void(Entity *)> &operation);
    void traverse(const std::function<void(const Entity *)> &operation) const;

    Matrix4x4 *worldTransform();
    const Matrix4x4 *worldTransform() const;
    bool isParentLessTransform() const { return m_parentLessTransform; }
    void setParentLessTransform(bool v) { m_parentLessTransform = v; }
    Sphere *localBoundingVolume() const { return m_localBoundingVolume.data(); }
    Sphere *worldBoundingVolume() const { return m_worldBoundingVolume.data(); }
    Sphere *worldBoundingVolumeWithChildren() const { return m_worldBoundingVolumeWithChildren.data(); }

    void addComponent(Qt3DCore::QNodeIdTypePair idAndType);
    void removeComponent(Qt3DCore::QNodeId nodeId);

    bool isBoundingVolumeDirty() const;
    void unsetBoundingVolumeDirty();

    void setTreeEnabled(bool enabled) { m_treeEnabled = enabled; }
    bool isTreeEnabled() const { return m_treeEnabled; }

    Qt3DCore::QNodeIdVector layerIds() const { return m_layerComponents + m_recursiveLayerComponents; }
    void addRecursiveLayerId(const Qt3DCore::QNodeId layerId);
    void removeRecursiveLayerId(const Qt3DCore::QNodeId layerId);
    void clearRecursiveLayerIds() { m_recursiveLayerComponents.clear(); }

    template<class Backend>
    Qt3DCore::QHandle<Backend> componentHandle() const
    {
        return Qt3DCore::QHandle<Backend>();
    }

    template<class Backend>
    QList<Qt3DCore::QHandle<Backend>> componentsHandle() const
    {
        return { };
    }

    template<class Backend>
    Backend *renderComponent() const
    {
        return nullptr;
    }

    template<class Backend>
    std::vector<Backend *> renderComponents() const
    {
        // We should never reach this, we expect specialization to have been
        // specified
        Q_UNREACHABLE_RETURN({});
    }

    template<class Backend>
    Qt3DCore::QNodeId componentUuid() const
    {
        return { };
    }

    template<class Backend>
    QList<Qt3DCore::QNodeId> componentsUuid() const
    {
        return { };
    }

    template<typename T>
    bool containsComponentsOfType() const
    {
        return !componentUuid<T>().isNull();
    }

    template<typename T, typename Ts, typename ... Ts2>
    bool containsComponentsOfType() const
    {
        return containsComponentsOfType<T>() && containsComponentsOfType<Ts, Ts2...>();
    }

protected:
    Q_DECLARE_PRIVATE(Entity)

private:
    NodeManagers *m_nodeManagers;
    HEntity m_handle;
    HEntity m_parentHandle;
    QList<HEntity> m_childrenHandles;

    HMatrix m_worldTransform;
    bool m_parentLessTransform;
    QSharedPointer<Sphere> m_localBoundingVolume;
    QSharedPointer<Sphere> m_worldBoundingVolume;
    QSharedPointer<Sphere> m_worldBoundingVolumeWithChildren;

    // Handles to Components
    Qt3DCore::QNodeId m_transformComponent;
    Qt3DCore::QNodeId m_materialComponent;
    Qt3DCore::QNodeId m_cameraComponent;
    QList<Qt3DCore::QNodeId> m_layerComponents;
    QList<Qt3DCore::QNodeId> m_levelOfDetailComponents;
    QList<Qt3DCore::QNodeId> m_rayCasterComponents;
    QList<Qt3DCore::QNodeId> m_shaderDataComponents;
    QList<Qt3DCore::QNodeId> m_lightComponents;
    QList<Qt3DCore::QNodeId> m_environmentLightComponents;
    Qt3DCore::QNodeId m_geometryRendererComponent;
    Qt3DCore::QNodeId m_pickingProxyComponent;
    Qt3DCore::QNodeId m_objectPickerComponent;
    Qt3DCore::QNodeId m_boundingVolumeDebugComponent;
    Qt3DCore::QNodeId m_computeComponent;
    Qt3DCore::QNodeId m_armatureComponent;

    // Includes recursive layers
    Qt3DCore::QNodeIdVector m_recursiveLayerComponents;

    QString m_objectName;
    bool m_boundingDirty;
    // true only if this and all parent nodes are enabled
    bool m_treeEnabled;
};

#define ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(Type, Handle) \
    /* Handle */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT Handle Entity::componentHandle<Type>() const; \
    /* Component */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT Type *Entity::renderComponent<Type>() const; \
    /* Uuid */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT Qt3DCore::QNodeId Entity::componentUuid<Type>() const;


#define ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(Type, Handle) \
    /* Handle */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT QList<Handle> Entity::componentsHandle<Type>() const; \
    /* Component */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT std::vector<Type *> Entity::renderComponents<Type>() const; \
    /* Uuid */ \
    template<> \
    Q_3DRENDERSHARED_PRIVATE_EXPORT Qt3DCore::QNodeIdVector Entity::componentsUuid<Type>() const;

#define ENTITY_COMPONENT_TEMPLATE_IMPL(Type, Handle, Manager, variable) \
    /* Handle */ \
    template<> \
    Handle Entity::componentHandle<Type>() const \
    { \
        return m_nodeManagers->lookupHandle<Type, Manager, Handle>(variable); \
    } \
    /* Component */ \
    template<> \
    Type *Entity::renderComponent<Type>() const \
    { \
        return m_nodeManagers->lookupResource<Type, Manager>(variable); \
    } \
    /* Uuid */ \
    template<> \
    Qt3DCore::QNodeId Entity::componentUuid<Type>() const \
    { \
        return variable; \
    }

#define ENTITY_COMPONENT_LIST_TEMPLATE_IMPL(Type, Handle, Manager, variable) \
    /* Handle */ \
    template<> \
    QList<Handle> Entity::componentsHandle<Type>() const \
    { \
        Manager *manager = m_nodeManagers->manager<Type, Manager>(); \
        QList<Handle> entries; \
        entries.reserve(variable.size()); \
        for (const QNodeId &id : variable) \
            entries.push_back(manager->lookupHandle(id)); \
        return entries; \
        } \
    /* Component */ \
    template<> \
    std::vector<Type *> Entity::renderComponents<Type>() const \
    { \
        Manager *manager = m_nodeManagers->manager<Type, Manager>(); \
        std::vector<Type *> entries; \
        entries.reserve(variable.size()); \
        for (const QNodeId &id : variable) \
            entries.push_back(manager->lookupResource(id)); \
        return entries; \
    } \
    /* Uuid */ \
    template<> \
    Qt3DCore::QNodeIdVector Entity::componentsUuid<Type>() const \
    { \
        return variable; \
    }

ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(Material, HMaterial)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(CameraLens, HCamera)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(Transform, HTransform)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(GeometryRenderer, HGeometryRenderer)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(ObjectPicker, HObjectPicker)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(PickingProxy, HPickingProxy)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(ComputeCommand, HComputeCommand)
ENTITY_COMPONENT_TEMPLATE_SPECIALIZATION(Armature, HArmature)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(Layer, HLayer)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(LevelOfDetail, HLevelOfDetail)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(RayCaster, HRayCaster)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(ShaderData, HShaderData)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(Light, HLight)
ENTITY_COMPONENT_LIST_TEMPLATE_SPECIALIZATION(EnvironmentLight, HEnvironmentLight)

class Q_AUTOTEST_EXPORT RenderEntityFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderEntityFunctor(AbstractRenderer *renderer, NodeManagers *manager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    NodeManagers *m_nodeManagers;
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ENTITY_H
