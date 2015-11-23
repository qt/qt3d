/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/qhandle_p.h>
#include <QVector>

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

class Q_AUTOTEST_EXPORT Entity : public Qt3DCore::QBackendNode
{
public:
    Entity();
    ~Entity();
    void cleanup();

    void setParentHandle(HEntity parentHandle);
    void setNodeManagers(NodeManagers *manager);
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void updateFromPeer(Qt3DCore::QNode *peer) Q_DECL_OVERRIDE;

    void dump() const;

    void  setHandle(HEntity handle);
    HEntity handle() const { return m_handle; }
    Entity *parent() const;
    HEntity parentHandle() const { return m_parentHandle; }

    void appendChildHandle(HEntity childHandle);
    void removeChildHandle(HEntity childHandle);
    QVector<HEntity> childrenHandles() const { return m_childrenHandles; }
    QVector<Entity *> children() const;
    bool hasChildren() const { return !m_childrenHandles.empty(); }

    QMatrix4x4 *worldTransform();
    const QMatrix4x4 *worldTransform() const;
    Sphere *localBoundingVolume() const { return m_localBoundingVolume.data(); }
    Sphere *worldBoundingVolume() const { return m_worldBoundingVolume.data(); }
    Sphere *worldBoundingVolumeWithChildren() const { return m_worldBoundingVolumeWithChildren.data(); }

    void addComponent(Qt3DCore::QComponent *component);
    void removeComponent(const Qt3DCore::QNodeId &nodeId);

    bool isEnabled() const;
    void setEnabled(bool isEnabled);

    bool isBoundingVolumeDirty() const;
    void unsetBoundingVolumeDirty();

    template<class Backend, uint INDEXBITS>
    Qt3DCore::QHandle<Backend, INDEXBITS> componentHandle() const
    {
        return Qt3DCore::QHandle<Backend, INDEXBITS>();
    }

    template<class Backend, uint INDEXBITS>
    QList<Qt3DCore::QHandle<Backend, INDEXBITS> > componentsHandle() const
    {
        return QList<Qt3DCore::QHandle<Backend, INDEXBITS> >();
    }

    template<class Backend>
    Backend *renderComponent() const
    {
        return Q_NULLPTR;
    }

    template<class Backend>
    QList<Backend *> renderComponents() const
    {
        return QList<Backend *>();
    }

    template<class Backend>
    Qt3DCore::QNodeId componentUuid() const
    {
        return Qt3DCore::QNodeId();
    }

    template<class Backend>
    QList<Qt3DCore::QNodeId> componentsUuid() const
    {
        return QList<Qt3DCore::QNodeId>();
    }

private:
    NodeManagers *m_nodeManagers;
    HEntity m_handle;
    HEntity m_parentHandle;
    QVector<HEntity > m_childrenHandles;

    HMatrix m_worldTransform;
    QSharedPointer<Sphere> m_localBoundingVolume;
    QSharedPointer<Sphere> m_worldBoundingVolume;
    QSharedPointer<Sphere> m_worldBoundingVolumeWithChildren;

    // Handles to Components
    Qt3DCore::QNodeId m_transformComponent;
    Qt3DCore::QNodeId m_materialComponent;
    Qt3DCore::QNodeId m_cameraComponent;
    QList<Qt3DCore::QNodeId> m_layerComponents;
    QList<Qt3DCore::QNodeId> m_shaderDataComponents;
    QList<Qt3DCore::QNodeId> m_lightComponents;
    Qt3DCore::QNodeId m_geometryRendererComponent;
    Qt3DCore::QNodeId m_objectPickerComponent;
    Qt3DCore::QNodeId m_boundingVolumeDebugComponent;

    QString m_objectName;
    bool m_enabled;
    bool m_boundingDirty;
};

// Handles
template<>
HMaterial Entity::componentHandle<Material>() const;

template<>
HCamera Entity::componentHandle<CameraLens>() const;

template<>
HTransform Entity::componentHandle<Transform>() const;

template<>
Q_AUTOTEST_EXPORT HGeometryRenderer Entity::componentHandle<GeometryRenderer>() const;

template<>
Q_AUTOTEST_EXPORT HObjectPicker Entity::componentHandle<ObjectPicker>() const;

template<>
QList<HLayer> Entity::componentsHandle<Layer>() const;

template<>
QList<HShaderData> Entity::componentsHandle<ShaderData>() const;

template<>
Q_AUTOTEST_EXPORT HBoundingVolumeDebug Entity::componentHandle<BoundingVolumeDebug>() const;

template<>
QList<HLight> Entity::componentsHandle<Light>() const;

// Render components
template<>
Material *Entity::renderComponent<Material>() const;

template<>
CameraLens *Entity::renderComponent<CameraLens>() const;

template<>
Transform *Entity::renderComponent<Transform>() const;

template<>
Q_AUTOTEST_EXPORT GeometryRenderer *Entity::renderComponent<GeometryRenderer>() const;

template<>
Q_AUTOTEST_EXPORT ObjectPicker *Entity::renderComponent<ObjectPicker>() const;

template<>
QList<Layer *> Entity::renderComponents<Layer>() const;

template<>
QList<ShaderData *> Entity::renderComponents<ShaderData>() const;

template<>
Q_AUTOTEST_EXPORT BoundingVolumeDebug *Entity::renderComponent<BoundingVolumeDebug>() const;

template<>
QList<Light *> Entity::renderComponents<Light>() const;

// UUid
template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<Transform>() const;

template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<CameraLens>() const;

template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<Material>() const;

template<>
Q_AUTOTEST_EXPORT QList<Qt3DCore::QNodeId> Entity::componentsUuid<Layer>() const;

template<>
Q_AUTOTEST_EXPORT QList<Qt3DCore::QNodeId> Entity::componentsUuid<ShaderData>() const;

template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<GeometryRenderer>() const;

template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<ObjectPicker>() const;

template<>
Q_AUTOTEST_EXPORT Qt3DCore::QNodeId Entity::componentUuid<BoundingVolumeDebug>() const;

template<>
Q_AUTOTEST_EXPORT QList<Qt3DCore::QNodeId> Entity::componentsUuid<Light>() const;

class RenderEntityFunctor : public Qt3DCore::QBackendNodeFunctor
{
public:
    explicit RenderEntityFunctor(NodeManagers *manager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    Qt3DCore::QBackendNode *get(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;

private:
    NodeManagers *m_nodeManagers;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ENTITY_H
