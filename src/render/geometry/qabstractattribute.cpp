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

namespace Qt3DRender {

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

/*!
 * \qmltype AbstractAttribute
 * \instantiates Qt3DRender::QAbstractAttribute
 * \inqmlmodule Qt3D.Render
 * \brief Uncreatable
 */

/*!
 * \class Qt3DRender::QAbstractAttribute
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DCore::QNode
 *
 * Abstract class.
 */

/*!
 * \typedef Qt3DRender::QAbstractBufferPtr
 * \relates Qt3DRender::QAbstractAttribute
 */

/*!
 * \fn QVector<QVector4D> Qt3DRender::QAbstractAttribute::asVector4D() const
 */

/*!
 * \fn QVector<QVector3D> Qt3DRender::QAbstractAttribute::asVector3D() const
 */

/*!
 * \fn QVector<QVector2D> Qt3DRender::QAbstractAttribute::asVector2D() const
 */

/*!
 * \fn void Qt3DRender::QAbstractAttribute::dump(int count)
 * \a count
 */

/*!
 * \enum QAbstractAttribute::AttributeType
 *
 * The type of the attribute.
 *
 * \value VertexAttribute
 * \value IndexAttribute
 */

/*!
 * \enum QAbstractAttribute::DataType
 *
 * The type of the data.
 *
 * \value Byte
 * \value UnsignedByte
 * \value Short
 * \value UnsignedShort
 * \value Int
 * \value UnsignedInt
 * \value HalfFloat
 * \value Float
 * \value Double
 */

/*!
 * Constructs a new QAbstractAttribute with \a parent.
 */
QAbstractAttribute::QAbstractAttribute(QNode *parent)
    : QNode(*new QAbstractAttributePrivate(), parent)
{
}

/*!
 * Destroys the abstract attribute.
 */
QAbstractAttribute::~QAbstractAttribute()
{
    Q_ASSERT_X(Qt3DCore::QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3DRender::QAbstractAttribute subclass didn't call QNode::cleanup in its destructor");
}

/*!
 * Constructs a new QAbstractAttribute from \a buf of \a type, \a dataSize, \a count, \a offset,
 * and \a stride with \a parent.
 */
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

/*!
 * \internal
 */
QAbstractAttribute::QAbstractAttribute(QAbstractAttributePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
 * \internal
 */
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

/*!
 * \internal
 */
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

/*!
 * \property QAbstractAttribute::buffer
 *
 * Holds the buffer.
 */
QAbstractBuffer *QAbstractAttribute::buffer() const
{
    Q_D(const QAbstractAttribute);
    return d->m_buffer;
}

/*!
 * \property QAbstractAttribute::name
 *
 * Holds the name.
 */
QString QAbstractAttribute::name() const
{
    Q_D(const QAbstractAttribute);
    return d->m_name;
}

/*!
 * \property QAbstractAttribute::dataSize
 *
 * Holds the data size.
 */
uint QAbstractAttribute::dataSize() const
{
    Q_D(const QAbstractAttribute);
    return d->m_dataSize;
}

/*!
 * \property QAbstractAttribute::dataType
 *
 * Holds the data type.
 */
QAbstractAttribute::DataType QAbstractAttribute::dataType() const
{
    Q_D(const QAbstractAttribute);
    return d->m_dataType;
}

/*!
 * \property QAbstractAttribute::count
 *
 * Holds the count.
 */
uint QAbstractAttribute::count() const
{
    Q_D(const QAbstractAttribute);
    return d->m_count;
}

/*!
 * \property QAbstractAttribute::byteStride
 *
 * Holds the byte stride.
 */
uint QAbstractAttribute::byteStride() const
{
    Q_D(const QAbstractAttribute);
    return d->m_byteStride;
}

/*!
 * \property QAbstractAttribute::byteOffset
 *
 * Holds the byte offset.
 */
uint QAbstractAttribute::byteOffset() const
{
    Q_D(const QAbstractAttribute);
    return d->m_byteOffset;
}

/*!
 * \property QAbstractAttribute::divisor
 *
 * Holds the divisor.
 */
uint QAbstractAttribute::divisor() const
{
    Q_D(const QAbstractAttribute);
    return d->m_divisor;
}

/*!
 * \property QAbstractAttribute::attributeType
 *
 * Holds the attribute type.
 */
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

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (buffer && !buffer->parent())
        buffer->setParent(this);

    d->m_buffer = buffer;
    emit bufferChanged(buffer);
}

void QAbstractAttribute::setName(const QString &name)
{
    Q_D(QAbstractAttribute);
    if (d->m_name == name)
        return;

    d->m_name = name;
    emit nameChanged(name);
}

void QAbstractAttribute::setDataType(DataType type)
{
    Q_D(QAbstractAttribute);

    if (d->m_dataType == type)
        return;

    d->m_dataType = type;
    emit dataTypeChanged(type);
}

void QAbstractAttribute::setDataSize(uint size)
{
    Q_D(QAbstractAttribute);
    if (d->m_dataSize == size)
        return;
    Q_ASSERT(size >= 1 && size <= 4);
    d->m_dataSize = size;
    emit dataSizeChanged(size);
}

void QAbstractAttribute::setCount(uint count)
{
    Q_D(QAbstractAttribute);
    if (d->m_count == count)
        return;

    d->m_count = count;
    emit countChanged(count);
}

void QAbstractAttribute::setByteStride(uint byteStride)
{
    Q_D(QAbstractAttribute);
    if (d->m_byteStride == byteStride)
        return;

    d->m_byteStride = byteStride;
    emit byteStrideChanged(byteStride);
}

void QAbstractAttribute::setByteOffset(uint byteOffset)
{
    Q_D(QAbstractAttribute);
    if (d->m_byteOffset == byteOffset)
        return;

    d->m_byteOffset = byteOffset;
    emit byteOffsetChanged(byteOffset);
}

void QAbstractAttribute::setDivisor(uint divisor)
{
    Q_D(QAbstractAttribute);
    if (d->m_divisor == divisor)
        return;

    d->m_divisor = divisor;
    emit divisorChanged(divisor);
}

void QAbstractAttribute::setAttributeType(AttributeType attributeType)
{
    Q_D(QAbstractAttribute);
    if (d->m_attributeType == attributeType)
        return;

    d->m_attributeType = attributeType;
    emit attributeTypeChanged(attributeType);
}

} // Qt3D

QT_END_NAMESPACE
