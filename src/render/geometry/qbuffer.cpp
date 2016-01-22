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

#include "qbuffer.h"
#include "qbuffer_p.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QBufferPrivate::QBufferPrivate()
    : QAbstractBufferPrivate()
    , m_usage(QBuffer::StaticDraw)
    , m_sync(false)
{
}

/*!
 * \qmltype Buffer
 * \instantiates Qt3DRender::QBuffer
 * \inqmlmodule Qt3D.Render
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
 */

/*!
 * \qmlproperty bool Buffer::sync
 *
 * Holds the sync flag. When sync is true, this will force data created
 * by a Qt3DRender::QBufferFunctor to also be updated on the frontend
 * Qt3DRender::QBuffer node. By default sync is false.
 *
 * \note: This has no effect if the buffer's data was set directly using the data
 * property.
 */

/*!
 * \class Qt3DRender::QBuffer
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DRender::QAbstractBuffer
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
 * \typedef Qt3DRender::QBufferFunctorPtr
 * \relates QBuffer
 */

/*!
 * Constructs a new QBuffer of buffer type \a ty with \a parent.
 */
QBuffer::QBuffer(QBuffer::BufferType ty, QNode *parent)
    : QAbstractBuffer(*new QBufferPrivate(), parent)
{
    Q_D(QBuffer);
    d->m_type = ty;
}

/*!
 * Destroys this buffer.
 */
QBuffer::~QBuffer()
{
    QAbstractBuffer::cleanup();
}

/*!
 * \internal
 */
QBuffer::QBuffer(QBufferPrivate &dd, QBuffer::BufferType ty, QNode *parent)
    : QAbstractBuffer(dd, parent)
{
    Q_D(QBuffer);
    d->m_type = ty;
}

/*!
 * \internal
 */
void QBuffer::copy(const QNode *ref)
{
    QAbstractBuffer::copy(ref);
    const QBuffer *buffer = static_cast<const QBuffer *>(ref);
    d_func()->m_type = buffer->d_func()->m_type;
    d_func()->m_usage = buffer->d_func()->m_usage;
    d_func()->m_functor = buffer->d_func()->m_functor;
    d_func()->m_sync = buffer->d_func()->m_sync;
}

/*!
 * \internal
 */
void QBuffer::sceneChangeEvent(const QSceneChangePtr &change)
{
    QScenePropertyChangePtr e = qSharedPointerCast<QScenePropertyChange>(change);
    if (e->type() == NodeUpdated && e->propertyName() == QByteArrayLiteral("data")) {
        const bool blocked = blockNotifications(true);
        setData(e->value().toByteArray());
        blockNotifications(blocked);
    }
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
 */
QBuffer::BufferType QBuffer::type() const
{
    Q_D(const QBuffer);
    return d->m_type;
}

/*!
 * Sets the buffer \a functor.
 */
void QBuffer::setBufferFunctor(const QBufferFunctorPtr &functor)
{
    Q_D(QBuffer);
    if (functor && d->m_functor && *functor == *d->m_functor)
        return;
    d->m_functor = functor;
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        change->setPropertyName("bufferFunctor");
        change->setValue(QVariant::fromValue(d->m_functor));
        d->notifyObservers(change);
    }
}

/*!
 * \return the buffer functor.
 */
QBufferFunctorPtr QBuffer::bufferFunctor() const
{
    Q_D(const QBuffer);
    return d->m_functor;
}

/*!
 * \property QBuffer::sync
 *
 * Holds the sync flag. When sync is true, this will force data created
 * by a Qt3DRender::QBufferFunctor to also be updated on the frontend
 * Qt3DRender::QBuffer node. By default sync is false.
 *
 * \note: This has no effect if the buffer's data was set directly using the data
 * property.
 */
void QBuffer::setSync(bool sync)
{
    Q_D(QBuffer);
    if (d->m_sync != sync) {
        d->m_sync = sync;
        emit syncChanged(sync);
    }
}

bool QBuffer::isSync() const
{
    Q_D(const QBuffer);
    return d->m_sync;
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
