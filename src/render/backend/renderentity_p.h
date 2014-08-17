/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QT3D_RENDER_RENDERNODE_H
#define QT3D_RENDER_RENDERNODE_H

#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/private/meshmanager_p.h>
#include <Qt3DRenderer/private/renderlayer_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/layermanager_p.h>
#include <Qt3DRenderer/private/lightmanager_p.h>
#include <Qt3DRenderer/private/cameramanager_p.h>
#include <Qt3DRenderer/private/rendertransform_p.h>
#include <Qt3DRenderer/private/transformmanager_p.h>
#include <Qt3DRenderer/private/materialmanager_p.h>

#include <Qt3DCore/qobserverinterface.h>
#include <Qt3DCore/qhandle.h>
#include <QVector>
#include <QUuid>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

namespace Qt3D {

class RendererAspect;
class Sphere;
class QNode;
class QEntity;
class QComponent;
class QTransform;

typedef QHandle<QMatrix4x4, 16> HMatrix;

namespace Render {
class RenderEntity;
}

typedef QHandle<Render::RenderEntity, 16> HEntity;

namespace Render {

class Renderer;
class EntityManager;

class RenderEntity : public QObserverInterface
{
public:
    RenderEntity();
    ~RenderEntity();
    void cleanup();

    void setParentHandle(HEntity parentHandle);
    void setRenderer(Renderer *renderer);
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void setPeer(QEntity *peer);

    void dump() const;

    void  setHandle(HEntity handle);
    HEntity handle() const { return m_handle; }
    RenderEntity *parent() const;
    HEntity parentHandle() const { return m_parentHandle; }

    void appendChildHandle(HEntity childHandle);
    void removeChildHandle(HEntity childHandle);
    QVector<HEntity> childrenHandles() const { return m_childrenHandles; }
    QVector<RenderEntity *> children() const;

    QMatrix4x4 *worldTransform();
    Sphere *localBoundingVolume() { return m_localBoundingVolume; }
    Sphere *worldBoundingVolume() { return m_worldBoundingVolume; }
    QUuid entityUuid() const { return m_frontendUuid; }

    void addComponent(QComponent *component);
    void removeComponent(QComponent *component);

    template<class Backend, int INDEXBITS>
    QHandle<Backend, INDEXBITS> componentHandle() const
    {
        return QHandle<Backend, INDEXBITS>();
    }

    template<class Backend>
    Backend* renderComponent() const
    {
        return Q_NULLPTR;
    }

    template<class Backend>
    QUuid componentUuid() const
    {
        return QUuid();
    }

    template<class Backend>
    QList<QUuid> componentUuidInTree() const
    {
        QList<QUuid> componentList;

        QUuid componentId = componentUuid<Backend>();
        if (!componentId.isNull())
            componentList.append(componentId);

        Q_FOREACH (RenderEntity *child, children())
            componentList.append(child->componentUuidInTree<Backend>());

        return componentList;
    }

    template<class Backend, int INDEXBITS>
    QList<QHandle<Backend, INDEXBITS> > componentHandlesInTree() const
    {
        QList<QHandle<Backend, INDEXBITS> > handles;

        QHandle<Backend, INDEXBITS> h = componentHandle<Backend, INDEXBITS>();
        if (!h.isNull())
            handles.append(h);

        Q_FOREACH (RenderEntity *child, children())
            handles.append(child->componentHandlesInTree<Backend, INDEXBITS>());

        return handles;
    }

    template<class Backend>
    QList<Backend *> renderComponentsInTree() const
    {
        QList<Backend *> components;

        Backend *component = renderComponent<Backend>();
        if (component != Q_NULLPTR)
            components.append(component);

        Q_FOREACH (RenderEntity *child, children())
            components.append(child->renderComponentsInTree<Backend>());

        return components;
    }

private:

    template<class Frontend, class Backend, class Manager>
    void createRenderComponentHelper(Frontend *frontend, Manager *manager)
    {
        // We index using the Frontend uuid
        if (!manager->contains(frontend->uuid())) {
            Backend *backend = manager->getOrCreateResource(frontend->uuid());
            backend->setRenderer(m_renderer);
            backend->setPeer(frontend);
        }
    }

    Renderer *m_renderer;
    HEntity m_handle;
    HEntity m_parentHandle;
    QVector<HEntity > m_childrenHandles;

    HMatrix m_worldTransform;
    Sphere *m_localBoundingVolume;
    Sphere *m_worldBoundingVolume;

    QUuid m_frontendUuid;

    // Handles to Components
    QUuid m_transformComponent;
    QUuid m_meshComponent;
    QUuid m_materialComponent;
    QUuid m_layerComponent;
    QUuid m_lightComponent;
    QUuid m_cameraComponent;

    QString m_objectName;
};

template<>
HMesh RenderEntity::componentHandle<RenderMesh>() const;

template<>
RenderMesh *RenderEntity::renderComponent<RenderMesh>() const;

template<>
HMaterial RenderEntity::componentHandle<RenderMaterial>() const;

template<>
RenderMaterial *RenderEntity::renderComponent<RenderMaterial>() const;

template<>
HLayer RenderEntity::componentHandle<RenderLayer>() const;

template<>
RenderLayer *RenderEntity::renderComponent<RenderLayer>() const;

template<>
HLight RenderEntity::componentHandle<RenderLight>() const;

template<>
RenderLight *RenderEntity::renderComponent<RenderLight>() const;

template<>
HCamera RenderEntity::componentHandle<RenderCameraLens>() const;

template<>
RenderCameraLens *RenderEntity::renderComponent<RenderCameraLens>() const;

template<>
HTransform RenderEntity::componentHandle<RenderTransform>() const;

template<>
RenderTransform *RenderEntity::renderComponent<RenderTransform>() const;

template<>
QUuid RenderEntity::componentUuid<RenderTransform>() const;

template<>
QUuid RenderEntity::componentUuid<RenderCameraLens>() const;

template<>
QUuid RenderEntity::componentUuid<RenderLayer>() const;

template<>
QUuid RenderEntity::componentUuid<RenderMaterial>() const;

template<>
QUuid RenderEntity::componentUuid<RenderLight>() const;

template<>
QUuid RenderEntity::componentUuid<RenderMesh>() const;

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERNODE_H
