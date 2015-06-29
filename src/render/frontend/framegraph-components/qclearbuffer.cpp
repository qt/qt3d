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

#include "qclearbuffer.h"
#include <private/qclearbuffer_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QClearBufferPrivate
    \internal
*/
QClearBufferPrivate::QClearBufferPrivate()
    : QFrameGraphNodePrivate()
    , m_buffersType(QClearBuffer::None)
{
}

void QClearBuffer::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QClearBuffer *b = static_cast<const QClearBuffer*>(ref);
    d_func()->m_buffersType = b->d_func()->m_buffersType;
}

QClearBuffer::QClearBuffer(QNode *parent)
    : QFrameGraphNode(*new QClearBufferPrivate, parent)
{
}

QClearBuffer::~QClearBuffer()
{
    QNode::cleanup();
}

/*! \internal */
QClearBuffer::QClearBuffer(QClearBufferPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}


QClearBuffer::BufferType QClearBuffer::buffers() const
{
    Q_D(const QClearBuffer);
    return d->m_buffersType;
}

void QClearBuffer::setBuffers(QClearBuffer::BufferType buffers)
{
    Q_D(QClearBuffer);
    if (d->m_buffersType != buffers) {
        d->m_buffersType = buffers;
        emit buffersChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
