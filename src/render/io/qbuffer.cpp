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
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::BufferPrivate
    \internal
*/
QBufferPrivate::QBufferPrivate()
    : QAbstractBufferPrivate()
    , m_usage(QBuffer::StaticDraw)
{
}


QBuffer::QBuffer(QBuffer::BufferType ty, QNode *parent)
    : QAbstractBuffer(*new QBufferPrivate(), parent)
{
    Q_D(QBuffer);
    d->m_type = ty;
}

QBuffer::~QBuffer()
{
    QAbstractBuffer::cleanup();
}

/*! \internal */
QBuffer::QBuffer(QBufferPrivate &dd, QBuffer::BufferType ty, QNode *parent)
    : QAbstractBuffer(dd, parent)
{
    Q_D(QBuffer);
    d->m_type = ty;
}

void QBuffer::copy(const QNode *ref)
{
    QAbstractBuffer::copy(ref);
    const QBuffer *buffer = static_cast<const QBuffer *>(ref);
    d_func()->m_type = buffer->d_func()->m_type;
    d_func()->m_usage = buffer->d_func()->m_usage;
    d_func()->m_functor = buffer->d_func()->m_functor;
}

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
        emit usageChanged();
    }
}

QBuffer::BufferType QBuffer::type() const
{
    Q_D(const QBuffer);
    return d->m_type;
}

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

QBufferFunctorPtr QBuffer::bufferFunctor() const
{
    Q_D(const QBuffer);
    return d->m_functor;
}

void QBuffer::setType(QBuffer::BufferType type)
{
    Q_D(QBuffer);
    if (type != d->m_type) {
        d->m_type = type;
        emit typeChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
