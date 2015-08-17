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

#include "renderattribute_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qbuffer.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderAttribute::RenderAttribute()
    : QBackendNode(ReadOnly)
    , m_dataType(QAbstractAttribute::Float)
    , m_dataSize(1)
    , m_count(0)
    , m_byteStride(0)
    , m_byteOffset(0)
    , m_divisor(0)
    , m_attributeType(QAbstractAttribute::VertexAttribute)
    , m_attributeDirty(false)
{
}

RenderAttribute::~RenderAttribute()
{
}

void RenderAttribute::cleanup()
{
    m_dataType = QAbstractAttribute::Float;
    m_dataSize = 1;
    m_count = 0;
    m_byteStride = 0;
    m_byteOffset = 0;
    m_divisor = 0;
    m_attributeType = QAbstractAttribute::VertexAttribute;
    m_bufferId = QNodeId();
    m_name.clear();
    m_attributeDirty = false;
}

void RenderAttribute::updateFromPeer(QNode *peer)
{
    QAttribute *attribute = static_cast<QAttribute *>(peer);
    if (attribute) {
        m_dataType = attribute->dataType();
        m_dataSize = attribute->dataSize();
        m_count = attribute->count();
        m_byteOffset = attribute->byteOffset();
        m_byteStride = attribute->byteStride();
        m_divisor = attribute->divisor();
        m_attributeType = attribute->attributeType();
        m_name = attribute->name();
        if (attribute->buffer())
            m_bufferId = attribute->buffer()->id();
        m_attributeDirty = true;
    }
}

void RenderAttribute::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    QByteArray propertyName = propertyChange->propertyName();

    switch (e->type()) {

    case NodeUpdated: {
        if (propertyName == QByteArrayLiteral("name")) {
            m_name = propertyChange->value().value<QString>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("dataType")) {
            m_dataType = static_cast<QAbstractAttribute::DataType>(propertyChange->value().value<int>());
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("dataSize")) {
            m_dataSize = propertyChange->value().value<uint>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("count")) {
            m_count = propertyChange->value().value<uint>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("byteStride")) {
            m_byteStride = propertyChange->value().value<uint>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("byteOffset")) {
            m_byteOffset = propertyChange->value().value<uint>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("divisor")) {
            m_divisor = propertyChange->value().value<uint>();
            m_attributeDirty = true;
        } else if (propertyName == QByteArrayLiteral("attributeType")) {
            m_attributeType = static_cast<QAbstractAttribute::AttributeType>(propertyChange->value().value<int>());
            m_attributeDirty = true;
        }
        break;
    }

    case NodeAdded: {
        if (propertyName == QByteArrayLiteral("buffer")) {
            m_bufferId = propertyChange->value().value<QNodeId>();
            m_attributeDirty = true;
        }
        break;
    }

    case NodeRemoved: {
        if (propertyName == QByteArrayLiteral("buffer")) {
            m_bufferId = QNodeId();
            m_attributeDirty = true;
        }
        break;
    }

    default:
        break;
    }
}

void RenderAttribute::unsetDirty()
{
    m_attributeDirty = false;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
