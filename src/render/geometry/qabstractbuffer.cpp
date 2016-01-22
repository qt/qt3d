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

#include "qabstractbuffer.h"
#include "qabstractbuffer_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {


QAbstractBufferPrivate::QAbstractBufferPrivate()
    : QNodePrivate()
{
}

/*!
 * \qmltype AbstractBuffer
 * \instantiates Qt3DRender::QAbstractBuffer
 * \inqmlmodule Qt3D.Render
 * \brief Uncreatable
 */

/*!
 * \class Qt3DRender::QAbstractBuffer
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DCore::QNode
 *
 * Abstract class.
 */

/*!
 * \fn void Qt3DRender::QAbstractBuffer::dataChanged(const QByteArray &bytes)
 *
 * This signal is emitted with \a bytes when data changes.
 */

/*!
 * Constructs a new QAbstractBuffer with \a parent.
 */
QAbstractBuffer::QAbstractBuffer(QNode *parent)
    : QNode(*new QAbstractBufferPrivate(), parent)
{
}

/*!
 * Destroys the abstract buffer.
 */
QAbstractBuffer::~QAbstractBuffer()
{
    Q_ASSERT_X(Qt3DCore::QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3DRender::QAbstractBuffer subclass didn't call QNode::cleanup in its destructor");
}

/*!
 * \internal
 */
QAbstractBuffer::QAbstractBuffer(QAbstractBufferPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
 * \internal
 */
void QAbstractBuffer::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractBuffer *buffer = static_cast<const QAbstractBuffer *>(ref);
    d_func()->m_data = buffer->d_func()->m_data;
}

/*!
 * Sets \a bytes as data.
 */
void QAbstractBuffer::setData(const QByteArray &bytes)
{
    Q_D(QAbstractBuffer);
    if (bytes != d->m_data) {
        d->m_data = bytes;
        Qt3DCore::QNodePrivate::get(this)->notifyPropertyChange("data", QVariant::fromValue(d->m_data));
        emit dataChanged(bytes);
    }
}

/*!
 * \return the data.
 */
QByteArray QAbstractBuffer::data() const
{
    Q_D(const QAbstractBuffer);
    return d->m_data;
}

} // Qt3DRender

QT_END_NAMESPACE
