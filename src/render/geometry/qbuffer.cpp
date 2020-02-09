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

#include "qbuffer.h"
#include "qbuffer_p.h"
#include <Qt3DRender/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QBufferPrivate::QBufferPrivate()
    : QNodePrivate()
    , m_type(QBuffer::VertexBuffer)
    , m_usage(QBuffer::StaticDraw)
    , m_syncData(false)
    , m_access(QBuffer::Write)
{
}

void QBufferPrivate::setData(const QByteArray &data)
{
    Q_Q(QBuffer);
    const bool blocked = q->blockNotifications(true);
    m_data = data;
    emit q->dataChanged(data);
    q->blockNotifications(blocked);
}

/*!
 * \qmltype Buffer
 * \instantiates Qt3DRender::QBuffer
 * \inqmlmodule Qt3D.Render
 *
 * \brief Provides a data store for raw data to later be used as vertices or
 * uniforms.
 */

/*!
 * \qmlproperty QBuffer::UsageType Buffer::usage
 *
 * Holds the buffer usage.
 */

/*!
 * \qmlproperty QBuffer::BufferType Buffer::type
 *
 * Holds the buffer type.
 *
 * \deprecated
 */

/*!
 * \qmlproperty bool Buffer::syncData
 *
 * Holds the syncData flag. When syncData is true, this will force data created
 * by a Qt3DRender::QBufferDataGenerator to also be updated on the frontend
 * Qt3DRender::QBuffer node. By default syncData is false.
 *
 * \note: This has no effect if the buffer's data was set directly using the data
 * property.
 */

/*!
 * \class Qt3DRender::QBuffer
 * \inheaderfile Qt3DRender/QBuffer
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DCore::QNode
 *
 * \brief Provides a data store for raw data to later be used as vertices or
 * uniforms.
 *
 * Data can be provided directly using QBuffer::setData().
 */

/*!
 * \fn void Qt3DRender::QBuffer::dataChanged(const QByteArray &bytes)
 *
 * This signal is emitted with \a bytes when data changes.
 */

/*!
 * \fn void Qt3DRender::QBuffer::dataAvailable()
 *
 * This signal is emitted when data becomes available.
 */

/*!
 * \enum QBuffer::BufferType
 *
 * The type of the buffer.
 *
 * \value VertexBuffer
 *        GL_ARRAY_BUFFER
 * \value IndexBuffer
 *        GL_ELEMENT_ARRAY_BUFFER
 * \value PixelPackBuffer
 *        GL_PIXEL_PACK_BUFFER
 * \value PixelUnpackBuffer
 *        GL_PIXEL_UNPACK_BUFFER
 * \value UniformBuffer
 *        GL_UNIFORM_BUFFER
 * \value ShaderStorageBuffer
 *        GL_SHADER_STORAGE_BUFFER
 * \value DrawIndirectBuffer
 *        GL_DRAW_INDIRECT_BUFFER
 *
 * \deprecated
 */

/*!
 * \enum QBuffer::UsageType
 *
 * The type of the usage.
 *
 * \value StreamDraw
 *        GL_STREAM_DRAW
 * \value StreamRead
 *        GL_STREAM_READ
 * \value StreamCopy
 *        GL_STREAM_COPY
 * \value StaticDraw
 *        GL_STATIC_DRAW
 * \value StaticRead
 *        GL_STATIC_READ
 * \value StaticCopy
 *        GL_STATIC_COPY
 * \value DynamicDraw
 *        GL_DYNAMIC_DRAW
 * \value DynamicRead
 *        GL_DYNAMIC_READ
 * \value DynamicCopy
 *        GL_DYNAMIC_COPY
 */

/*!
 * \enum QBuffer::AccessType
 *
 * \value Write
 *        Write access
 * \value Read
 *        Read access
 * \value ReadWrite
 *        Write|Read
 */

/*!
 * \typedef Qt3DRender::QBufferDataGeneratorPtr
 * \relates Qt3DRender::QBuffer
 */

/*!
 * Constructs a new QBuffer with \a parent.
 */
QBuffer::QBuffer(QNode *parent)
    : QNode(*new QBufferPrivate(), parent)
{
}

/*!
 * Constructs a new QBuffer of buffer type \a ty with \a parent.
 *
 * \deprecated
 */
QBuffer::QBuffer(QBuffer::BufferType ty, QNode *parent)
    : QNode(*new QBufferPrivate(), parent)
{
    Q_D(QBuffer);
    d->m_type = ty;
}

/*!
 * \internal
 */
QBuffer::~QBuffer()
{
}

/*!
 * Sets \a bytes as data.
 */
void QBuffer::setData(const QByteArray &bytes)
{
    Q_D(QBuffer);
    if (bytes != d->m_data) {
        d->setData(bytes);
        d->update();
    }
}

/*!
 * Updates the data by replacing it with \a bytes at \a offset.
 */
void QBuffer::updateData(int offset, const QByteArray &bytes)
{
    Q_D(QBuffer);
    Q_ASSERT(offset >= 0 && (offset + bytes.size()) <= d->m_data.size());

    // Update data
    d->m_data.replace(offset, bytes.size(), bytes);
    const bool blocked = blockNotifications(true);
    emit dataChanged(d->m_data);
    blockNotifications(blocked);

    QBufferUpdate updateData;
    updateData.offset = offset;
    updateData.data = bytes;
    setProperty("QT3D_updateData", QVariant::fromValue(updateData));
    d->update();
}

/*!
 * \return the data.
 */
QByteArray QBuffer::data() const
{
    Q_D(const QBuffer);
    return d->m_data;
}

/*!
 * \property QBuffer::usage
 *
 * Holds the buffer usage.
 */
QBuffer::UsageType QBuffer::usage() const
{
    Q_D(const QBuffer);
    return d->m_usage;
}

void QBuffer::setUsage(QBuffer::UsageType usage)
{
    Q_D(QBuffer);
    if (usage != d->m_usage) {
        d->m_usage = usage;
        emit usageChanged(usage);
    }
}

/*!
 * \property QBuffer::type
 *
 * Holds the buffer type.
 *
 * \deprecated
 */
QBuffer::BufferType QBuffer::type() const
{
    Q_D(const QBuffer);
    return d->m_type;
}

/*!
 * \property QBuffer::syncData
 *
 * Holds the syncData flag. When syncData is true, this will force data created
 * by a Qt3DRender::QBufferDataGenerator to also be updated on the frontend
 * Qt3DRender::QBuffer node. By default syncData is false.
 *
 * \note: This has no effect if the buffer's data was set directly using the data
 * property.
 */
void QBuffer::setSyncData(bool syncData)
{
    Q_D(QBuffer);
    if (d->m_syncData != syncData) {
        d->m_syncData = syncData;
        emit syncDataChanged(syncData);
    }
}

void QBuffer::setAccessType(QBuffer::AccessType access)
{
    Q_D(QBuffer);
    if (d->m_access != access) {
        d->m_access = access;
        Q_EMIT accessTypeChanged(access);
    }
}

bool QBuffer::isSyncData() const
{
    Q_D(const QBuffer);
    return d->m_syncData;
}

/*!
 * \property Qt3DRender::QBuffer::accessType
 *
 * Returns the \l {QBuffer::}{AccessType} of the buffer.
 *
 * \sa QBuffer::AccessType
 */
QBuffer::AccessType QBuffer::accessType() const
{
    Q_D(const QBuffer);
    return d->m_access;
}

void QBuffer::setType(QBuffer::BufferType type)
{
    Q_D(QBuffer);
    if (type != d->m_type) {
        d->m_type = type;
        emit typeChanged(type);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
