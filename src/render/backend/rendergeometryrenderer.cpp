/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "rendergeometryrenderer_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/private/geometryrenderermanager_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderGeometryRenderer::RenderGeometryRenderer()
    : QBackendNode(ReadWrite)
    , m_instanceCount(0)
    , m_primitiveCount(0)
    , m_baseVertex(0)
    , m_baseInstance(0)
    , m_restartIndex(-1)
    , m_primitiveRestart(false)
    , m_primitiveType(QGeometryRenderer::Triangles)
    , m_dirty(false)
    , m_manager(Q_NULLPTR)
{
}

RenderGeometryRenderer::~RenderGeometryRenderer()
{
}

void RenderGeometryRenderer::cleanup()
{
    m_instanceCount = 0;
    m_primitiveCount = 0;
    m_baseVertex = 0;
    m_baseInstance = 0;
    m_restartIndex = -1;
    m_primitiveRestart = false;
    m_primitiveType = QGeometryRenderer::Triangles;
    m_geometryId = QNodeId();
    m_dirty = false;
    m_functor.reset();
}

void RenderGeometryRenderer::setManager(GeometryRendererManager *manager)
{
    m_manager = manager;
}

void RenderGeometryRenderer::updateFromPeer(QNode *peer)
{
    QGeometryRenderer *geometryRenderer = static_cast<QGeometryRenderer *>(peer);
    if (geometryRenderer) {
        m_instanceCount = geometryRenderer->instanceCount();
        m_primitiveCount = geometryRenderer->primitiveCount();
        m_baseVertex = geometryRenderer->baseVertex();
        m_baseInstance = geometryRenderer->baseInstance();
        m_restartIndex = geometryRenderer->restartIndex();
        m_primitiveRestart = geometryRenderer->primitiveRestart();
        m_primitiveType = geometryRenderer->primitiveType();
        if (geometryRenderer->geometry() != Q_NULLPTR)
            m_geometryId = geometryRenderer->geometry()->id();
        m_functor = geometryRenderer->geometryFunctor();
        if (m_functor && m_manager != Q_NULLPTR)
            m_manager->addDirtyGeometryRenderer(peerUuid());
        m_dirty = true;
    }
}

void RenderGeometryRenderer::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    QByteArray propertyName = propertyChange->propertyName();

    switch (e->type()) {

    case NodeUpdated: {
        if (propertyName == QByteArrayLiteral("instanceCount")) {
            m_instanceCount = propertyChange->value().value<int>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("primitiveCount")) {
            m_primitiveCount = propertyChange->value().value<int>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("baseVertex")) {
            m_baseVertex = propertyChange->value().value<int>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("baseInstance")) {
            m_baseInstance = propertyChange->value().value<int>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("restartIndex")) {
            m_restartIndex = propertyChange->value().value<int>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("primitiveRestart")) {
            m_primitiveRestart = propertyChange->value().value<bool>();
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("primitiveType")) {
            m_primitiveType = static_cast<QGeometryRenderer::PrimitiveType>(propertyChange->value().value<int>());
            m_dirty = true;
        } else if (propertyName == QByteArrayLiteral("geometryFunctor")) {
            QGeometryFunctorPtr newFunctor = propertyChange->value().value<QGeometryFunctorPtr>();
            m_dirty |= !(newFunctor && m_functor && *newFunctor == *m_functor);
            m_functor = newFunctor;
            if (m_functor && m_manager != Q_NULLPTR)
                m_manager->addDirtyGeometryRenderer(peerUuid());
        }
        break;
    }

    case NodeAdded: {
        if (propertyName == QByteArrayLiteral("geometry")) {
            m_geometryId = propertyChange->value().value<QNodeId>();
            m_dirty = true;
        }
        break;
    }

    case NodeRemoved: {
        if (propertyName == QByteArrayLiteral("geometry")) {
            m_geometryId = QNodeId();
            m_dirty = true;
        }
        break;
    }

    default:
        break;
    }

    // Add to dirty list in manager
}

void RenderGeometryRenderer::executeFunctor()
{
    Q_ASSERT(m_functor);
    QGeometry *geometry = (*m_functor)();

    QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
    e->setPropertyName("geometry");
    // The Frontend element has to perform the clone
    // So that the objects are created in the main thread
    e->setValue(QVariant::fromValue(geometry));
    e->setTargetNode(peerUuid());
    notifyObservers(e);
    // Maybe we could also send a status to help troubleshoot errors
}

void RenderGeometryRenderer::unsetDirty()
{
    m_dirty = false;
}

RenderGeometryRendererFunctor::RenderGeometryRendererFunctor(GeometryRendererManager *manager)
    : m_manager(manager)
{
}

QBackendNode *RenderGeometryRendererFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    RenderGeometryRenderer *geometryRenderer = m_manager->getOrCreateResource(frontend->id());
    geometryRenderer->setFactory(factory);
    geometryRenderer->setManager(m_manager);
    geometryRenderer->setPeer(frontend);
    return geometryRenderer;
}

QBackendNode *RenderGeometryRendererFunctor::get(const QNodeId &id) const
{
    return m_manager->lookupResource(id);
}

void RenderGeometryRendererFunctor::destroy(const QNodeId &id) const
{
    return m_manager->releaseResource(id);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
