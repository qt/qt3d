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

#include "qabstractbuffer.h"
#include "qabstractbuffer_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DCore::QAbstractBufferPrivate
    \internal
*/

QAbstractBufferPrivate::QAbstractBufferPrivate()
    : QNodePrivate()
{
}

QAbstractBuffer::QAbstractBuffer(QNode *parent)
    : QNode(*new QAbstractBufferPrivate(), parent)
{
}

QAbstractBuffer::~QAbstractBuffer()
{
    Q_ASSERT_X(Qt3DCore::QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3DRender::QAbstractBuffer subclass didn't call QNode::cleanup in its destructor");
}

/*! \internal */
QAbstractBuffer::QAbstractBuffer(QAbstractBufferPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QAbstractBuffer::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QAbstractBuffer *buffer = static_cast<const QAbstractBuffer *>(ref);
    d_func()->m_data = buffer->d_func()->m_data;
}

void QAbstractBuffer::setData(const QByteArray &bytes)
{
    Q_D(QAbstractBuffer);
    if (bytes != d->m_data) {
        d->m_data = bytes;
        Qt3DCore::QNodePrivate::get(this)->notifyPropertyChange("data", QVariant::fromValue(d->m_data));
        emit dataChanged(bytes);
    }
}

QByteArray QAbstractBuffer::data() const
{
    Q_D(const QAbstractBuffer);
    return d->m_data;
}

} // Qt3DRender

QT_END_NAMESPACE
