/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qclearbuffer.h"
#include <private/qclearbuffer_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QClearBufferPrivate::QClearBufferPrivate(QClearBuffer *qq)
    : QFrameGraphItemPrivate(qq)
    , m_buffersType(QClearBuffer::None)
{
}

void QClearBuffer::copy(const QNode *ref)
{
    QFrameGraphItem::copy(ref);
    const QClearBuffer *b = static_cast<const QClearBuffer*>(ref);
    d_func()->m_buffersType = b->d_func()->m_buffersType;

    Q_FOREACH (QFrameGraphItem *fgChild, b->d_func()->m_fgChildren)
        appendFrameGraphItem(qobject_cast<QFrameGraphItem *>(QNodePrivate::get(fgChild)->clone()));
}

QClearBuffer::QClearBuffer(QNode *parent)
    : QFrameGraphItem(*new QClearBufferPrivate(this), parent)
{
}

QClearBuffer::QClearBuffer(QClearBufferPrivate &dd, QNode *parent)
    : QFrameGraphItem(dd, parent)
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
