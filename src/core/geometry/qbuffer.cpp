// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qbuffer.h"
#include "qbuffer_p.h"
#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

const char *QBufferPrivate::UpdateDataPropertyName = "QT3D_updateData";

QBufferPrivate::QBufferPrivate()
    : QNodePrivate()
    , m_usage(QBuffer::StaticDraw)
    , m_access(QBuffer::Write)
    , m_dirty(false)
{
}

QBufferPrivate *QBufferPrivate::get(QBuffer *q)
{
    return q->d_func();
}

void QBufferPrivate::update()
{
    if (!m_blockNotifications) {
        m_dirty = true;
        markDirty(QScene::BuffersDirty);
    }
    QNodePrivate::update();
}

void QBufferPrivate::setData(const QByteArray &data)
{
    // this is called when date is loaded from backend, should not set dirty flag
    Q_Q(QBuffer);
    const bool blocked = q->blockNotifications(true);
    m_data = data;
    emit q->dataChanged(data);
    q->blockNotifications(blocked);
}

/*!
 * \qmltype Buffer
 * \instantiates Qt3DCore::QBuffer
 * \inqmlmodule Qt3D.Core
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
 * \class Qt3DCore::QBuffer
 * \inheaderfile Qt3DCore/QBuffer
 * \inmodule Qt3DCore
 *
 * \inherits Qt3DCore::QNode
 *
 * \brief Provides a data store for raw data to later be used as vertices or
 * uniforms.
 *
 * Data can be provided directly using QBuffer::setData().
 */

/*!
 * \fn void Qt3DCore::QBuffer::dataChanged(const QByteArray &bytes)
 *
 * This signal is emitted with \a bytes when data changes.
 */

/*!
 * \fn void Qt3DCore::QBuffer::dataAvailable()
 *
 * This signal is emitted when data becomes available.
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
 * Constructs a new QBuffer with \a parent.
 */
QBuffer::QBuffer(QNode *parent)
    : QNode(*new QBufferPrivate(), parent)
{
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

    QVariantList updateDataList;
    const QVariant propertyData = property(QBufferPrivate::UpdateDataPropertyName);
    if (propertyData.isValid())
        updateDataList = propertyData.toList();
    updateDataList.push_back(QVariant::fromValue(updateData));

    setProperty(QBufferPrivate::UpdateDataPropertyName, updateDataList);
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

void QBuffer::setAccessType(QBuffer::AccessType access)
{
    Q_D(QBuffer);
    if (d->m_access != access) {
        d->m_access = access;
        Q_EMIT accessTypeChanged(access);
    }
}

/*!
 * \property Qt3DCore::QBuffer::accessType
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

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qbuffer.cpp"
