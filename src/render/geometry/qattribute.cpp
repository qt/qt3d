/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qattribute.h"
#include "qattribute_p.h"
#include <Qt3DRender/qbuffer.h>

#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QAttributePrivate::QAttributePrivate()
    : QNodePrivate()
    , m_buffer(nullptr)
    , m_name()
    , m_dataType(QAttribute::Float)
    , m_dataSize(1)
    , m_count(0)
    , m_byteStride(0)
    , m_byteOffset(0)
    , m_divisor(0)
    , m_attributeType(QAttribute::VertexAttribute)
{
}

/*!
 * \qmltype Attribute
 * \instantiates Qt3DRender::QAttribute
 * \inqmlmodule Qt3D.Render
 * \brief Uncreatable
 */

/*!
 * \class Qt3DRender::QAttribute
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DCore::QNode
 *
 */

/*!
 * \typedef Qt3DRender::QBufferPtr
 * \relates Qt3DRender::QAttribute
 */

/*!
 * \enum QAttribute::AttributeType
 *
 * The type of the attribute.
 *
 * \value VertexAttribute
 * \value IndexAttribute
 */

/*!
 * \enum QAttribute::VertexBaseType
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
 * Constructs a new QAttribute with \a parent.
 */
QAttribute::QAttribute(QNode *parent)
    : QNode(*new QAttributePrivate(), parent)
{
}

/*!
 * Constructs a new QAttribute from \a buf of \a type, \a dataSize, \a count, \a offset,
 * and \a stride with \a parent.
 */
QAttribute::QAttribute(QBuffer *buf, VertexBaseType type, uint dataSize, uint count, uint offset, uint stride, QNode *parent)
    : QNode(*new QAttributePrivate(), parent)
{
    Q_D(QAttribute);
    setBuffer(buf);
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_dataType = type;
    d->m_dataSize = dataSize;
    d->m_byteStride = stride;
}


/*!
 * Constructs a new QAttribute named \a name from \a buf of \a type, \a
 * dataSize, \a count, \a offset, and \a stride with \a parent.
 */
QAttribute::QAttribute(QBuffer *buf, const QString &name, VertexBaseType type, uint dataSize, uint count, uint offset, uint stride, QNode *parent)
    : QNode(*new QAttributePrivate(), parent)
{
    Q_D(QAttribute);
    setBuffer(buf);
    d->m_name = name;
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_dataType = type;
    d->m_dataSize = dataSize;
    d->m_byteStride = stride;
}

/*! \internal */
QAttribute::~QAttribute()
{
}

/*!
 * \property QAttribute::buffer
 *
 * Holds the buffer.
 */
QBuffer *QAttribute::buffer() const
{
    Q_D(const QAttribute);
    return d->m_buffer;
}

/*!
 * \property QAttribute::name
 *
 * Holds the name.
 */
QString QAttribute::name() const
{
    Q_D(const QAttribute);
    return d->m_name;
}

/*!
 * \property QAttribute::vertexSize
 *
 * Holds the data size.
 */
uint QAttribute::vertexSize() const
{
    Q_D(const QAttribute);
    return d->m_dataSize;
}

/*!
 * \property QAttribute::vertexBaseType
 *
 * Holds the data type.
 */
QAttribute::VertexBaseType QAttribute::vertexBaseType() const
{
    Q_D(const QAttribute);
    return d->m_dataType;
}

/*!
 * \property QAttribute::count
 *
 * Holds the count.
 */
uint QAttribute::count() const
{
    Q_D(const QAttribute);
    return d->m_count;
}

/*!
 * \property QAttribute::byteStride
 *
 * Holds the byte stride.
 */
uint QAttribute::byteStride() const
{
    Q_D(const QAttribute);
    return d->m_byteStride;
}

/*!
 * \property QAttribute::byteOffset
 *
 * Holds the byte offset.
 */
uint QAttribute::byteOffset() const
{
    Q_D(const QAttribute);
    return d->m_byteOffset;
}

/*!
 * \property QAttribute::divisor
 *
 * Holds the divisor.
 */
uint QAttribute::divisor() const
{
    Q_D(const QAttribute);
    return d->m_divisor;
}

/*!
 * \property QAttribute::attributeType
 *
 * Holds the attribute type.
 */
QAttribute::AttributeType QAttribute::attributeType() const
{
    Q_D(const QAttribute);
    return d->m_attributeType;
}

void QAttribute::setBuffer(QBuffer *buffer)
{
    Q_D(QAttribute);
    if (d->m_buffer == buffer)
        return;

    if (d->m_buffer)
        d->unregisterDestructionHelper(d->m_buffer);

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (buffer && !buffer->parent())
        buffer->setParent(this);

    d->m_buffer = buffer;

    // Ensures proper bookkeeping
    if (d->m_buffer)
        d->registerDestructionHelper(d->m_buffer, &QAttribute::setBuffer, d->m_buffer);

    emit bufferChanged(buffer);
}

void QAttribute::setName(const QString &name)
{
    Q_D(QAttribute);
    if (d->m_name == name)
        return;

    d->m_name = name;
    emit nameChanged(name);
}

void QAttribute::setDataType(VertexBaseType type)
{
    Q_D(QAttribute);

    if (d->m_dataType == type)
        return;

    d->m_dataType = type;
    emit dataTypeChanged(type);
}

void QAttribute::setDataSize(uint size)
{
    Q_D(QAttribute);
    if (d->m_dataSize == size)
        return;
    Q_ASSERT(size >= 1 && size <= 4);
    d->m_dataSize = size;
    emit dataSizeChanged(size);
}

void QAttribute::setCount(uint count)
{
    Q_D(QAttribute);
    if (d->m_count == count)
        return;

    d->m_count = count;
    emit countChanged(count);
}

void QAttribute::setByteStride(uint byteStride)
{
    Q_D(QAttribute);
    if (d->m_byteStride == byteStride)
        return;

    d->m_byteStride = byteStride;
    emit byteStrideChanged(byteStride);
}

void QAttribute::setByteOffset(uint byteOffset)
{
    Q_D(QAttribute);
    if (d->m_byteOffset == byteOffset)
        return;

    d->m_byteOffset = byteOffset;
    emit byteOffsetChanged(byteOffset);
}

void QAttribute::setDivisor(uint divisor)
{
    Q_D(QAttribute);
    if (d->m_divisor == divisor)
        return;

    d->m_divisor = divisor;
    emit divisorChanged(divisor);
}

void QAttribute::setAttributeType(AttributeType attributeType)
{
    Q_D(QAttribute);
    if (d->m_attributeType == attributeType)
        return;

    d->m_attributeType = attributeType;
    emit attributeTypeChanged(attributeType);
}
/*!
 * \brief QAttribute::defaultPositionAttributeName
 * \return the name of the default position attribute
 */
QString QAttribute::defaultPositionAttributeName()
{
    return QStringLiteral("vertexPosition");
}
/*!
 * \brief QAttribute::defaultNormalAttributeName
 * \return the name of the default normal attribute
 */
QString QAttribute::defaultNormalAttributeName()
{
    return QStringLiteral("vertexNormal");
}
/*!
 * \brief QAttribute::defaultColorAttributeName
 * \return the name of the default color attribute
 */
QString QAttribute::defaultColorAttributeName()
{
    return QStringLiteral("vertexColor");
}
/*!
 * \brief QAttribute::defaultTextureCoordinateAttributeName
 * \return the name of the default texture coordinate attribute
 */
QString QAttribute::defaultTextureCoordinateAttributeName()
{
    return QStringLiteral("vertexTexCoord");
}
/*!
 * \brief QAttribute::defaultTangentAttributeName
 * \return the name of the default tangent attribute
 */
QString QAttribute::defaultTangentAttributeName()
{
    return QStringLiteral("vertexTangent");
}

Qt3DCore::QNodeCreatedChangeBasePtr QAttribute::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QAttributeData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAttribute);
    data.bufferId = qIdForNode(d->m_buffer);
    data.name = d->m_name;
    data.dataType = d->m_dataType;
    data.dataSize = d->m_dataSize;
    data.count = d->m_count;
    data.byteStride = d->m_byteStride;
    data.byteOffset = d->m_byteOffset;
    data.divisor = d->m_divisor;
    data.attributeType = d->m_attributeType;
    return creationChange;
}

} // Qt3DRender

QT_END_NAMESPACE
