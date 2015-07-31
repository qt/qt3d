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

#include "renderbuffer_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderBuffer::RenderBuffer()
    : QBackendNode(QBackendNode::ReadOnly)
    , m_type(QBuffer::VertexBuffer)
    , m_usage(QBuffer::StaticDraw)
    , m_bufferDirty(false)
{
    // Maybe it could become read write if we want to inform
    // the frontend QBuffer node of any backend issue
}

RenderBuffer::~RenderBuffer()
{
}

void RenderBuffer::cleanup()
{
    m_type = QBuffer::VertexBuffer;
    m_usage = QBuffer::StaticDraw;
    m_data.clear();
    m_bufferDirty = false;
}

void RenderBuffer::updateFromPeer(QNode *peer)
{
    QBuffer *buffer = static_cast<QBuffer *>(peer);
    if (buffer != Q_NULLPTR) {
        m_type = buffer->type();
        m_usage = buffer->usage();
        m_data = buffer->data();
        m_bufferDirty = true;
    }
}

void RenderBuffer::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        QByteArray propertyName = propertyChange->propertyName();
        if (propertyName == QByteArrayLiteral("data")) {
            m_data = propertyChange->value().value<QByteArray>();
            m_bufferDirty = true;
        } else if (propertyName == QByteArrayLiteral("type")) {
            m_type = static_cast<QBuffer::BufferType>(propertyChange->value().value<int>());
            m_bufferDirty = true;
        } else if (propertyName == QByteArrayLiteral("usage")) {
            m_usage = static_cast<QBuffer::UsageType>(propertyChange->value().value<int>());
            m_bufferDirty = true;
        }
    }
}

void RenderBuffer::unsetDirty()
{
    m_bufferDirty = false;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
