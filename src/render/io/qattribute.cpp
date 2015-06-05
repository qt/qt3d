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

#include "qattribute.h"
#include "qattribute_p.h"
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <Qt3DCore/qabstractbuffer.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    class Qt3D::AttributePrivate
    \internal
*/
AttributePrivate::AttributePrivate()
    : QAbstractAttributePrivate()
{
}

Attribute::Attribute(QAbstractBufferPtr buf, int type, int count, int offset, int stride)
    : QAbstractAttribute(*new AttributePrivate, buf, type, count, offset, stride)
{
}

/*! \internal */
Attribute::Attribute(AttributePrivate &dd, QAbstractBufferPtr buf, int type, int count, int offset, int stride)
    : QAbstractAttribute(dd, buf, type, count, offset, stride)
{
}

QVector<QVector3D> Attribute::asVector3D() const
{
    Q_D(const Attribute);
    const QByteArray buffer = d->m_buffer->data();
    const char *rawBuffer = buffer.constData();
    rawBuffer += d->m_offset;
    const float* fptr;
    int stride;

    switch (type()) {
    case GL_FLOAT_VEC2:
        stride = sizeof(float) * 2; break;

    case GL_FLOAT_VEC3:
        stride = sizeof(float) * 3; break;

    case GL_FLOAT_VEC4:
        stride = sizeof(float) * 4; break;

    default:
        qCDebug(Render::Io) << Q_FUNC_INFO << "can't convert" << QString::number(type(), 16) << "to QVector3D";
        return QVector<QVector3D>();
    }

    if (d->m_stride != 0)
        stride = d->m_stride;
    QVector<QVector3D> result;
    result.resize(d->m_count);

    for (uint c=0; c < d->m_count; ++c) {
        QVector3D v;
        fptr = reinterpret_cast<const float*>(rawBuffer);

        switch (type()) {
        case GL_FLOAT_VEC2:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
            v.setZ(0.0f);
            break;

        case GL_FLOAT_VEC3:
        case GL_FLOAT_VEC4:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
            v.setZ(fptr[2]);
            break;

        default:
            break; // should never happen, we check types above
        }

        result[c] = v;
        rawBuffer += stride;
    }

    return result;
}

QVector<QVector2D> Attribute::asVector2D() const
{
    Q_D(const Attribute);
    char* rawBuffer = d->m_buffer->data().data();
    rawBuffer += d->m_offset;
    float* fptr;
    int stride;

    switch (type()) {
    case GL_FLOAT_VEC2:
        stride = sizeof(float) * 2; break;

    case GL_FLOAT_VEC3:
        stride = sizeof(float) * 3; break;

    case GL_FLOAT_VEC4:
        stride = sizeof(float) * 4; break;

    default:
        qCDebug(Render::Io) << Q_FUNC_INFO << "can't convert" << QString::number(type(), 16) << "to QVector2D";
        return QVector<QVector2D>();
    }

    if (d->m_stride != 0)
        stride = d->m_stride;

    QVector<QVector2D> result;
    result.resize(d->m_count);

    for (uint c=0; c < d->m_count; ++c) {
        QVector2D v;
        fptr = reinterpret_cast<float*>(rawBuffer);
        v.setX(fptr[0]);
        v.setY(fptr[1]);
        result[c] = v;
        rawBuffer += stride;
    }

    return result;
}

void Attribute::dump(int count)
{
    Q_D(const Attribute);
    char* rawBuffer = d->m_buffer->data().data();
    rawBuffer += d->m_offset;

    float* fptr;
    quint16* usptr;

    int stride = d->m_stride;

    for (int c=0; c<count; ++c) {
        switch (type()) {
        case GL_UNSIGNED_SHORT:
            if (!stride) stride = sizeof(quint16);
            usptr = reinterpret_cast<quint16*>(rawBuffer);
            qCDebug(Render::Io) << c << ":u16:" << usptr[0];
            break;
        case GL_UNSIGNED_INT:
            if (!stride) stride = sizeof(quint32);
            qCDebug(Render::Io) << c << ":u32:" << reinterpret_cast<quint32*>(rawBuffer)[0];
            break;
        case GL_FLOAT_VEC2:
            if (!stride) stride = sizeof(float) * 2;
            fptr = reinterpret_cast<float*>(rawBuffer);
            qCDebug(Render::Io) << c << ":vec2:"<< fptr[0] << fptr[0];
            break;

        case GL_FLOAT_VEC3:
            if (!stride) stride = sizeof(float) * 3;
            fptr = reinterpret_cast<float*>(rawBuffer);
            qCDebug(Render::Io) << c << ":vec3:" << fptr[0] << fptr[0] << fptr[2];
            break;

        default: qCDebug(Render::Io) << Q_FUNC_INFO << "unspported type:" << QString::number(type(), 16);
        }
    }
}

} // Qt3D

QT_END_NAMESPACE
