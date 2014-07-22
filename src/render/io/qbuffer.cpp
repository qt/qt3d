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

#include "qbuffer.h"
#include "qbuffer_p.h"
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

BufferPrivate::BufferPrivate(Buffer *qq)
    : QAbstractBufferPrivate(qq)
{
}

Buffer::Buffer(QOpenGLBuffer::Type ty)
    : QAbstractBuffer(*new BufferPrivate(this))
{
    Q_D(Buffer);
    d->m_type = ty;
    d->m_usage = QOpenGLBuffer::StaticDraw;
}


Buffer::Buffer(BufferPrivate &dd, QOpenGLBuffer::Type ty)
    : QAbstractBuffer(dd)
{
    Q_D(Buffer);
    d->m_type = ty;
    d->m_usage = QOpenGLBuffer::StaticDraw;
}

void Buffer::setUsage(QOpenGLBuffer::UsagePattern usage)
{
    Q_D(Buffer);
    d->m_usage = usage;
}

QOpenGLBuffer::Type Buffer::type() const
{
    Q_D(const Buffer);
    return d->m_type;
}

void Buffer::bind()
{

}

void Buffer::create()
{
    // TO DO -> Wrap createGL in here
}

QOpenGLBuffer Buffer::createGL() const
{
    Q_D(const Buffer);
    QOpenGLBuffer b(d->m_type);
    b.setUsagePattern(d->m_usage);
    if (!b.create())
        qCWarning(Render::Io) << Q_FUNC_INFO << "buffer creation failed";

    if (!b.bind())
        qCWarning(Render::Io) << Q_FUNC_INFO << "buffer binding failed";

    b.allocate(d->m_data.count());
    b.release();
    return b;
}

void Buffer::upload(QOpenGLBuffer b)
{
    Q_D(Buffer);
    if (!b.bind())
        qCWarning(Render::Io) << Q_FUNC_INFO << "buffer bind failed";
    b.allocate(NULL, d->m_data.count()); // orphan the buffer
    b.allocate(d->m_data.data(),
               d->m_data.count());
    b.release();
    qCDebug(Render::Io) << "uploaded buffer size=" << d->m_data.count();
}

} // Qt3D

QT_END_NAMESPACE
