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

#include "qabstractattribute.h"
#include "qabstractattribute_p.h"
#include <qabstractbuffer.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractAttributePrivate
    \internal
*/
QAbstractAttributePrivate::QAbstractAttributePrivate()
{
}

/*! \internal */
QAbstractAttribute::QAbstractAttribute(QAbstractAttributePrivate &dd, QAbstractBufferPtr buf, int type, uint count, uint offset, uint stride)
    : d_ptr(&dd)
{
    Q_D(QAbstractAttribute);
    d->m_buffer = buf;
    d->m_count = count;
    d->m_divisor = 0;
    d->m_offset = offset;
    d->m_type = type;
    d->m_stride = stride;
}

QAbstractAttribute::QAbstractAttribute(QAbstractBufferPtr buf, int type, uint count, uint offset, uint stride)
    : d_ptr(new QAbstractAttributePrivate)
{
    Q_D(QAbstractAttribute);
    d->m_buffer = buf;
    d->m_count = count;
    d->m_divisor = 0;
    d->m_offset = offset;
    d->m_type = type;
    d->m_stride = stride;
}

QAbstractAttribute::~QAbstractAttribute()
{
}

int QAbstractAttribute::type() const
{
    Q_D(const QAbstractAttribute);
    return d->m_type;
}

uint QAbstractAttribute::count() const
{
    Q_D(const QAbstractAttribute);
    return d->m_count;
}

uint QAbstractAttribute::byteStride() const
{
    Q_D(const QAbstractAttribute);
    return d->m_stride;
}

uint QAbstractAttribute::byteOffset()
{
    Q_D(const QAbstractAttribute);
    return d->m_offset;
}

uint QAbstractAttribute::divisor() const
{
    Q_D(const QAbstractAttribute);
    return d->m_divisor;
}

QAbstractBufferPtr QAbstractAttribute::buffer() const
{
    Q_D(const QAbstractAttribute);
    return d->m_buffer;
}

} // Qt3D

QT_END_NAMESPACE
