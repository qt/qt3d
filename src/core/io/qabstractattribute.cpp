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

#include "qabstractattribute.h"
#include "qabstractattribute_p.h"
#include "qabstractbuffer.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractAttributePrivate
    \internal
*/
QAbstractAttributePrivate::QAbstractAttributePrivate()
    : QNodePrivate()
    , m_buffer(Q_NULLPTR)
    , m_name()
    , m_dataType(QAbstractAttribute::Float)
    , m_dataSize(1)
    , m_count(0)
    , m_byteStride(0)
    , m_byteOffset(0)
    , m_divisor(0)
    , m_attributeType(QAbstractAttribute::VertexAttribute)
{
}

QAbstractAttribute::QAbstractAttribute(QNode *parent)
    : QNode(*new QAbstractAttributePrivate(), parent)
{
}

QAbstractAttribute::~QAbstractAttribute()
{
    Q_ASSERT_X(QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3D::QAbstractAttribute subclass didn't call QNode::cleanup in its destructor");
}

QAbstractAttribute::QAbstractAttribute(QAbstractBuffer *buf, DataType type, uint dataSize, uint count, uint offset, uint stride, QNode *parent)
    : QNode(*new QAbstractAttributePrivate(), parent)
{
    Q_D(QAbstractAttribute);
    setBuffer(buf);
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_dataType = type;
    d->m_dataSize = dataSize;
    d->m_byteStride = stride;
}

QAbstractAttribute::QAbstractAttribute(QAbstractAttributePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

QAbstractAttribute::QAbstractAttribute(QAbstractAttributePrivate &dd, QAbstractBuffer *buf, const QString &name, DataType dataType, uint dataSize, uint count, uint offset, uint stride, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QAbstractAttribute);
    setBuffer(buf);
    d->m_name = name;
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_dataType = dataType;
    d->m_dataSize = dataSize;
    d->m_byteStride = stride;
}

void QAbstractAttribute::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractAttribute *attribute = static_cast<const QAbstractAttribute *>(ref);
    d_func()->m_buffer = qobject_cast<QAbstractBuffer *>(QNode::clone(attribute->d_func()->m_buffer));
    d_func()->m_name = attribute->d_func()->m_name;
    d_func()->m_count = attribute->d_func()->m_count;
    d_func()->m_divisor = attribute->d_func()->m_divisor;
    d_func()->m_byteOffset = attribute->d_func()->m_byteOffset;
    d_func()->m_dataType = attribute->d_func()->m_dataType;
    d_func()->m_dataSize = attribute->d_func()->m_dataSize;
    d_func()->m_byteStride = attribute->d_func()->m_byteStride;
    d_func()->m_attributeType = attribute->d_func()->m_attributeType;
}

QAbstractBuffer *QAbstractAttribute::buffer() const
{
    Q_D(const QAbstractAttribute);
    return d->m_buffer;
}

QString QAbstractAttribute::name() const
{
    Q_D(const QAbstractAttribute);
    return d->m_name;
}

uint QAbstractAttribute::dataSize() const
{
    Q_D(const QAbstractAttribute);
    return d->m_dataSize;
}

QAbstractAttribute::DataType QAbstractAttribute::dataType() const
{
    Q_D(const QAbstractAttribute);
    return d->m_dataType;
}

uint QAbstractAttribute::count() const
{
    Q_D(const QAbstractAttribute);
    return d->m_count;
}

uint QAbstractAttribute::byteStride() const
{
    Q_D(const QAbstractAttribute);
    return d->m_byteStride;
}

uint QAbstractAttribute::byteOffset() const
{
    Q_D(const QAbstractAttribute);
    return d->m_byteOffset;
}

uint QAbstractAttribute::divisor() const
{
    Q_D(const QAbstractAttribute);
    return d->m_divisor;
}

QAbstractAttribute::AttributeType QAbstractAttribute::attributeType() const
{
    Q_D(const QAbstractAttribute);
    return d->m_attributeType;
}

void QAbstractAttribute::setBuffer(QAbstractBuffer *buffer)
{
    Q_D(QAbstractAttribute);
    if (d->m_buffer == buffer)
        return;

    if (d->m_buffer && d->m_changeArbiter) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("buffer");
        change->setValue(QVariant::fromValue(d->m_buffer->id()));
        d->notifyObservers(change);
    }

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (buffer && !buffer->parent())
        buffer->setParent(this);

    d->m_buffer = buffer;
    const bool blocked = blockNotifications(true);
    emit bufferChanged();
    blockNotifications(blocked);

    if (d->m_buffer && d->m_changeArbiter) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
        change->setPropertyName("buffer");
        change->setValue(QVariant::fromValue(buffer->id()));
        d->notifyObservers(change);
    }
}

void QAbstractAttribute::setName(const QString &name)
{
    Q_D(QAbstractAttribute);
    if (d->m_name == name)
        return;

    d->m_name = name;
    emit nameChanged();
}

void QAbstractAttribute::setDataType(DataType type)
{
    Q_D(QAbstractAttribute);

    if (d->m_dataType == type)
        return;

    d->m_dataType = type;
    emit dataTypeChanged();
}

void QAbstractAttribute::setDataSize(uint size)
{
    Q_D(QAbstractAttribute);
    if (d->m_dataSize == size)
        return;
    Q_ASSERT(size >= 1 && size <= 4);
    d->m_dataSize = size;
    emit dataSizeChanged();
}

void QAbstractAttribute::setCount(uint count)
{
    Q_D(QAbstractAttribute);
    if (d->m_count == count)
        return;

    d->m_count = count;
    emit countChanged();
}

void QAbstractAttribute::setByteStride(uint byteStride)
{
    Q_D(QAbstractAttribute);
    if (d->m_byteStride == byteStride)
        return;

    d->m_byteStride = byteStride;
    emit byteStrideChanged();
}

void QAbstractAttribute::setByteOffset(uint byteOffset)
{
    Q_D(QAbstractAttribute);
    if (d->m_byteOffset == byteOffset)
        return;

    d->m_byteOffset = byteOffset;
    emit byteOffsetChanged();
}

void QAbstractAttribute::setDivisor(uint divisor)
{
    Q_D(QAbstractAttribute);
    if (d->m_divisor == divisor)
        return;

    d->m_divisor = divisor;
    emit divisorChanged();
}

void QAbstractAttribute::setAttributeType(AttributeType attributeType)
{
    Q_D(QAbstractAttribute);
    if (d->m_attributeType == attributeType)
        return;

    d->m_attributeType = attributeType;
    emit attributeTypeChanged();
}

} // Qt3D

QT_END_NAMESPACE
