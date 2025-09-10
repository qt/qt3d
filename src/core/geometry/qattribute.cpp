// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qattribute.h"
#include "qattribute_p.h"
#include <Qt3DCore/qbuffer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QAttributePrivate::QAttributePrivate()
    : QNodePrivate()
    , m_buffer(nullptr)
    , m_name()
    , m_vertexBaseType(QAttribute::Float)
    , m_vertexSize(1)
    , m_count(0)
    , m_byteStride(0)
    , m_byteOffset(0)
    , m_divisor(0)
    , m_attributeType(QAttribute::VertexAttribute)
    , m_dirty(false)
{
}

void QAttributePrivate::update()
{
    if (!m_blockNotifications)
        m_dirty = true;
    QNodePrivate::update();
}

QAttributePrivate *QAttributePrivate::get(QAttribute *q)
{
    return q->d_func();
}

/*!
 * \qmltype Attribute
 * \nativetype Qt3DCore::QAttribute
 * \inqmlmodule Qt3D.Core
 * \brief Defines an attribute and how data should be read from a Buffer.
 *
 * There are 3 types of attributes.
 * \list
 * \li VertexAttribute: used to define data to be read on a per vertex basis
 * \li IndexAttribute: used to define vertex indices when indexed draw calls are
 * to be used
 *\li DrawIndirectAttribute: used to specify the DrawIndirect buffer to be used
 * when indirect draw calls are to be used
 * \endlist
 *
 * \note when an attribute is of type DrawIndirectAttribute, only count, stride
 * and offset are relevant.
 *
 * When providing your own attributes, it may make sense to name your attribute
 * using helpers such as QAttribute::defaultPositionAttributeName() as that
 * will ensure your geometry will be compatible with picking and the various
 * materials provided in the Qt3DExtras module.
 */

/*!
 * \class Qt3DCore::QAttribute
 * \inheaderfile Qt3DCore/QAttribute
 * \inmodule Qt3DCore
 *
 * \inherits Qt3DCore::QNode
 *
 * \brief Defines an attribute and how data should be read from a QBuffer.
 *
 * There are 3 types of attributes.
 * \list
 * \li VertexAttribute: used to define data to be read on a per vertex basis
 * \li IndexAttribute: used to define vertex indices when indexed draw calls are
 * to be used
 *\li DrawIndirectAttribute: used to specify the DrawIndirect buffer to be used
 * when indirect draw calls are to be used
 * \endlist
 *
 * \note when an attribute is of type DrawIndirectAttribute, only count, stride
 * and offset are relevant.
 *
 * When providing your own attributes, it may make sense to name your attribute
 * using helpers such as QAttribute::defaultPositionAttributeName() as that
 * will ensure your geometry will be compatible with picking and the various
 * materials provided in the Qt3DExtras module.
 *
 * \sa QBuffer
 */

/*!
 * \typedef Qt3DCore::QBufferPtr
 * \relates Qt3DCore::QAttribute
 */

/*!
 * \enum Qt3DCore::QAttribute::AttributeType
 *
 * The type of the attribute.
 *
 * \value VertexAttribute
 * \value IndexAttribute
 * \value DrawIndirectAttribute
 */

/*!
 * \enum Qt3DCore::QAttribute::VertexBaseType
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
    : QAttribute(parent)
{
    Q_D(QAttribute);
    setBuffer(buf);
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_vertexBaseType = type;
    d->m_vertexSize = dataSize;
    d->m_byteStride = stride;
}


/*!
 * Constructs a new QAttribute named \a name from \a buf of \a type, \a
 * dataSize, \a count, \a offset, and \a stride with \a parent.
 */
QAttribute::QAttribute(QBuffer *buf, const QString &name, VertexBaseType type, uint dataSize, uint count, uint offset, uint stride, QNode *parent)
    : QAttribute(parent)
{
    Q_D(QAttribute);
    setBuffer(buf);
    d->m_name = name;
    d->m_count = count;
    d->m_byteOffset = offset;
    d->m_vertexBaseType = type;
    d->m_vertexSize = dataSize;
    d->m_byteStride = stride;
}

/*! \internal */
QAttribute::~QAttribute()
{
}

/*!
 * \property Qt3DCore::QAttribute::buffer
 *
 * Holds the buffer.
 */
QBuffer *QAttribute::buffer() const
{
    Q_D(const QAttribute);
    return d->m_buffer;
}

/*!
 * \property Qt3DCore::QAttribute::name
 *
 * Holds the name.
 */
QString QAttribute::name() const
{
    Q_D(const QAttribute);
    return d->m_name;
}

/*!
 * \property Qt3DCore::QAttribute::vertexSize
 *
 * Holds the data size, it can only be 1 to 4 units (scalars and vectors),
 * 9 units (3x3 matrices) or 16 units (4x4 matrices).
 */
uint QAttribute::vertexSize() const
{
    Q_D(const QAttribute);
    return d->m_vertexSize;
}

/*!
 * \property Qt3DCore::QAttribute::vertexBaseType
 *
 * Holds the data type.
 */
QAttribute::VertexBaseType QAttribute::vertexBaseType() const
{
    Q_D(const QAttribute);
    return d->m_vertexBaseType;
}

/*!
 * \property Qt3DCore::QAttribute::count
 *
 * Holds the count.
 */
uint QAttribute::count() const
{
    Q_D(const QAttribute);
    return d->m_count;
}

/*!
 * \property Qt3DCore::QAttribute::byteStride
 *
 * Holds the byte stride.
 */
uint QAttribute::byteStride() const
{
    Q_D(const QAttribute);
    return d->m_byteStride;
}

/*!
 * \property Qt3DCore::QAttribute::byteOffset
 *
 * Holds the byte offset.
 */
uint QAttribute::byteOffset() const
{
    Q_D(const QAttribute);
    return d->m_byteOffset;
}

/*!
 * \property Qt3DCore::QAttribute::divisor
 *
 * Holds the divisor.
 */
uint QAttribute::divisor() const
{
    Q_D(const QAttribute);
    return d->m_divisor;
}

/*!
 * \property Qt3DCore::QAttribute::attributeType
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

void QAttribute::setVertexBaseType(VertexBaseType type)
{
    Q_D(QAttribute);

    if (d->m_vertexBaseType == type)
        return;

    d->m_vertexBaseType = type;
    emit vertexBaseTypeChanged(type);
    emit dataTypeChanged(type);
}

void QAttribute::setVertexSize(uint size)
{
    Q_D(QAttribute);
    if (d->m_vertexSize == size)
        return;
    Q_ASSERT((size >= 1 && size <= 4) || (size == 9) || (size == 16));
    d->m_vertexSize = size;
    emit vertexSizeChanged(size);
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

/*!
 * \brief QAttribute::defaultJointIndicesAttributeName
 * \return the name of the default joint indices attribute
 */
QString QAttribute::defaultJointIndicesAttributeName()
{
    return QStringLiteral("vertexJointIndices");
}

/*!
 * \brief QAttribute::defaultJointIndicesAttributeName
 * \return the name of the default joint weights attribute
 */
QString QAttribute::defaultJointWeightsAttributeName()
{
    return QStringLiteral("vertexJointWeights");
}

/*!
 * \brief QAttribute::defaultTextureCoordinate1AttributeName
 * \return the name of the default attribute for the second layer of texture
 * coordinates
 */
QString QAttribute::defaultTextureCoordinate1AttributeName()
{
    return QStringLiteral("vertexTexCoord1");
}

/*!
 * \brief QAttribute::defaultTextureCoordinate2AttributeName
 * \return the name of the default attribute for the third layer of texture
 * coordinates
 */
QString QAttribute::defaultTextureCoordinate2AttributeName()
{
    return QStringLiteral("vertexTexCoord2");
}

/*!
\fn Qt3DCore::QAttribute::dataSizeChanged(uint vertexSize)

The signal is emitted with \a vertexSize when the dataSize changes.
*/
/*!
\fn Qt3DCore::QAttribute::dataTypeChanged(Qt3DCore::QAttribute::VertexBaseType vertexBaseType)

The signal is emitted with \a vertexBaseType when the dataType changed.
*/

} // Qt3DCore

QT_END_NAMESPACE

#include "moc_qattribute.cpp"
