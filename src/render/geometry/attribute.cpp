/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "attribute_p.h"
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/private/qattribute_p.h>
#include <Qt3DRender/private/stringtoint_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Attribute::Attribute()
    : BackendNode(ReadOnly)
    , m_nameId(0)
    , m_vertexBaseType(QAttribute::Float)
    , m_vertexSize(1)
    , m_count(0)
    , m_byteStride(0)
    , m_byteOffset(0)
    , m_divisor(0)
    , m_attributeType(QAttribute::VertexAttribute)
    , m_attributeDirty(false)
{
}

Attribute::~Attribute()
{
}

void Attribute::cleanup()
{
    m_vertexBaseType = QAttribute::Float;
    m_vertexSize = 1;
    m_count = 0;
    m_byteStride = 0;
    m_byteOffset = 0;
    m_divisor = 0;
    m_attributeType = QAttribute::VertexAttribute;
    m_bufferId = Qt3DCore::QNodeId();
    m_name.clear();
    m_attributeDirty = false;
    m_nameId = 0;
}

void Attribute::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAttribute *node = qobject_cast<const QAttribute *>(frontEnd);
    if (!node)
        return;

    m_attributeDirty = firstTime;
    if (m_name != node->name()) {
        m_name = node->name();
        m_nameId = StringToInt::lookupId(m_name);
        m_attributeDirty = true;
    }
    if (m_vertexBaseType != node->vertexBaseType()) {
        m_vertexBaseType = node->vertexBaseType();
        m_attributeDirty = true;
    }
    if (m_vertexSize != node->vertexSize()) {
        m_vertexSize = node->vertexSize();
        m_attributeDirty = true;
    }
    if (m_count != node->count()) {
        m_count = node->count();
        m_attributeDirty = true;
    }
    if (m_byteStride != node->byteStride()) {
        m_byteStride = node->byteStride();
        m_attributeDirty = true;
    }
    if (m_byteOffset != node->byteOffset()) {
        m_byteOffset = node->byteOffset();
        m_attributeDirty = true;
    }
    if (m_divisor != node->divisor()) {
        m_divisor = node->divisor();
        m_attributeDirty = true;
    }
    if (m_attributeType != node->attributeType()) {
        m_attributeType = node->attributeType();
        m_attributeDirty = true;
    }
    const auto bufferId = node->buffer() ? node->buffer()->id() : QNodeId{};
    if (bufferId != m_bufferId) {
        m_bufferId = bufferId;
        m_attributeDirty = true;
    }

    markDirty(AbstractRenderer::AllDirty);
}

void Attribute::unsetDirty()
{
    m_attributeDirty = false;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
