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

#include "buffer_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRender/private/buffermanager_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Buffer::Buffer()
    : QBackendNode(QBackendNode::ReadWrite)
    , m_type(QBuffer::VertexBuffer)
    , m_usage(QBuffer::StaticDraw)
    , m_bufferDirty(false)
    , m_sync(false)
    , m_manager(Q_NULLPTR)
{
    // Maybe it could become read write if we want to inform
    // the frontend QBuffer node of any backend issue
}

Buffer::~Buffer()
{
}

void Buffer::cleanup()
{
    m_type = QBuffer::VertexBuffer;
    m_usage = QBuffer::StaticDraw;
    m_data.clear();
    m_functor.reset();
    m_bufferDirty = false;
    m_sync = false;
}


void Buffer::setManager(BufferManager *manager)
{
    m_manager = manager;
}

void Buffer::executeFunctor()
{
    Q_ASSERT(m_functor);
    m_data = (*m_functor)();
    if (m_sync) {
        // Send data back to the frontend
        QBackendScenePropertyChangePtr e(new QBackendScenePropertyChange(NodeUpdated, peerUuid()));
        e->setPropertyName("data");
        e->setTargetNode(peerUuid());
        e->setValue(QVariant::fromValue(m_data));
        notifyObservers(e);
    }
}

void Buffer::updateFromPeer(Qt3DCore::QNode *peer)
{
    QBuffer *buffer = static_cast<QBuffer *>(peer);
    if (buffer != Q_NULLPTR) {
        m_type = buffer->type();
        m_usage = buffer->usage();
        m_data = buffer->data();
        m_functor = buffer->bufferFunctor();
        // Add to dirty list in the manager
        if (m_functor && m_manager != Q_NULLPTR)
            m_manager->addDirtyBuffer(peerUuid());
        m_bufferDirty = true;
        m_sync = buffer->isSync();
    }
}

void Buffer::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QByteArray propertyName = propertyChange->propertyName();
        if (propertyName == QByteArrayLiteral("data")) {
            QByteArray newData = propertyChange->value().value<QByteArray>();
            m_bufferDirty |= m_data != newData;
            m_data = newData;
        } else if (propertyName == QByteArrayLiteral("type")) {
            m_type = static_cast<QBuffer::BufferType>(propertyChange->value().value<int>());
            m_bufferDirty = true;
        } else if (propertyName == QByteArrayLiteral("usage")) {
            m_usage = static_cast<QBuffer::UsageType>(propertyChange->value().value<int>());
            m_bufferDirty = true;
        } else if (propertyName == QByteArrayLiteral("bufferFunctor")) {
            QBufferFunctorPtr newFunctor = propertyChange->value().value<QBufferFunctorPtr>();
            m_bufferDirty |= !(newFunctor && m_functor && *newFunctor == *m_functor);
            m_functor = newFunctor;
            if (m_functor && m_manager != Q_NULLPTR)
                m_manager->addDirtyBuffer(peerUuid());
        } else if (propertyName == QByteArrayLiteral("sync")) {
            m_sync = propertyChange->value().toBool();
        }
    }
}

// Called by Renderer once the buffer has been uploaded to OpenGL
void Buffer::unsetDirty()
{
    m_bufferDirty = false;
}

BufferFunctor::BufferFunctor(BufferManager *manager)
    : m_manager(manager)
{
}

Qt3DCore::QBackendNode *BufferFunctor::create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const
{
    Buffer *buffer = m_manager->getOrCreateResource(frontend->id());
    buffer->setFactory(factory);
    buffer->setManager(m_manager);
    buffer->setPeer(frontend);
    return buffer;
}

Qt3DCore::QBackendNode *BufferFunctor::get(const Qt3DCore::QNodeId &id) const
{
    return m_manager->lookupResource(id);
}

void BufferFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    return m_manager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
