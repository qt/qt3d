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
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    class Qt3D::AttributePrivate
    \internal
*/
QAttributePrivate::QAttributePrivate()
    : QAbstractAttributePrivate()
{
}

QAttribute::QAttribute(QNode *parent)
    : QAbstractAttribute(*new QAttributePrivate(), parent)
{
}

QAttribute::QAttribute(QBuffer *buf, int type, int count, int offset, int stride)
    : QAbstractAttribute(*new QAttributePrivate(), buf, QString(), type, count, offset, stride)
{
}

QAttribute::QAttribute(QBuffer *buf, const QString &name, int type, int count, int offset, int stride)
    : QAbstractAttribute(*new QAttributePrivate(), buf, name, type, count, offset, stride)
{
}

QAttribute::~QAttribute()
{
    QAbstractAttribute::cleanup();
}

void QAttribute::copy(const QNode *ref)
{
    QAbstractAttribute::copy(ref);
}

QVector<QVector4D> QAttribute::asVector4D() const
{
    Q_D(const QAttribute);
    const QByteArray buffer = d->m_buffer->data();
    const char *rawBuffer = buffer.constData();
    rawBuffer += d->m_byteOffset;
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
        return QVector<QVector4D>();
    }

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;
    QVector<QVector4D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector4D v;
        fptr = reinterpret_cast<const float*>(rawBuffer);

        switch (type()) {
        case GL_FLOAT_VEC2:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
            break;

        case GL_FLOAT_VEC3:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
            v.setZ(fptr[2]);
            break;

        case GL_FLOAT_VEC4:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
            v.setZ(fptr[2]);
            v.setW(fptr[3]);
            break;

        default:
            break; // should never happen, we check types above
        }

        result[c] = v;
        rawBuffer += stride;
    }

    return result;
}

QVector<QVector3D> QAttribute::asVector3D() const
{
    Q_D(const QAttribute);
    const QByteArray buffer = d->m_buffer->data();
    const char *rawBuffer = buffer.constData();
    rawBuffer += d->m_byteOffset;
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

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;
    QVector<QVector3D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector3D v;
        fptr = reinterpret_cast<const float*>(rawBuffer);

        switch (type()) {
        case GL_FLOAT_VEC2:
            v.setX(fptr[0]);
            v.setY(fptr[1]);
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

QVector<QVector2D> QAttribute::asVector2D() const
{
    Q_D(const QAttribute);
    char* rawBuffer = d->m_buffer->data().data();
    rawBuffer += d->m_byteOffset;
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

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;

    QVector<QVector2D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector2D v;
        fptr = reinterpret_cast<float*>(rawBuffer);
        v.setX(fptr[0]);
        v.setY(fptr[1]);
        result[c] = v;
        rawBuffer += stride;
    }

    return result;
}

void QAttribute::dump(int count)
{
    Q_D(const QAttribute);
    const char* rawBuffer = d->m_buffer->data().data();
    rawBuffer += d->m_byteOffset;

    const float* fptr;
    const quint16* usptr;

    int stride = d->m_byteStride;

    for (int c=0; c<count; ++c) {
        switch (type()) {
        case GL_UNSIGNED_SHORT:
            if (stride == 0)
                stride = sizeof(quint16);
            usptr = reinterpret_cast<const quint16*>(rawBuffer + stride * c);
            qCDebug(Render::Io) << c << ":u16:" << usptr[0];
            break;
        case GL_UNSIGNED_INT:
            if (stride == 0)
                stride = sizeof(quint32);
            qCDebug(Render::Io) << c << ":u32:" << reinterpret_cast<const quint32*>(rawBuffer + stride * c)[0];
            break;
        case GL_FLOAT_VEC2:
            if (stride == 0)
                stride = sizeof(float) * 2;
            fptr = reinterpret_cast<const float*>(rawBuffer + stride * c);
            qCDebug(Render::Io) << c << ":vec2:"<< fptr[0] << fptr[1];
            break;

        case GL_FLOAT_VEC3:
            if (stride == 0)
                stride = sizeof(float) * 3;
            fptr = reinterpret_cast<const float*>(rawBuffer + stride * c);
            qCDebug(Render::Io) << c << ":vec3:" << fptr[0] << fptr[1] << fptr[2];
            break;

        default: qCDebug(Render::Io) << Q_FUNC_INFO << "unspported type:" << QString::number(type(), 16);
        }
    }
}

QBuffer *QAttribute::buffer() const
{
    return static_cast<QBuffer *>(QAbstractAttribute::buffer());
}

} // Qt3D

QT_END_NAMESPACE
