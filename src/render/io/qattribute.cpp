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

QAttribute::QAttribute(QBuffer *buf, DataType type, uint dataSize, int count, int offset, int stride)
    : QAbstractAttribute(*new QAttributePrivate(), buf, QString(), type, dataSize, count, offset, stride)
{
}

QAttribute::QAttribute(QBuffer *buf, const QString &name, DataType type, uint dataSize, int count, int offset, int stride)
    : QAbstractAttribute(*new QAttributePrivate(), buf, name, type, dataSize, count, offset, stride)
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

    switch (dataType()) {
    case QAttribute::Float:
        stride = sizeof(float) * dataSize();
        break;

    default:
        qCDebug(Render::Io) << Q_FUNC_INFO << "can't convert" << dataType() << "x" << dataSize() << "to QVector3D";
        return QVector<QVector4D>();
    }

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;
    QVector<QVector4D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector4D v;
        fptr = reinterpret_cast<const float*>(rawBuffer);

        for (uint i = 0, m = dataSize(); i < m; ++i)
            v[i] = fptr[i];

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

    switch (dataType()) {
    case QAttribute::Float:
        stride = sizeof(float) * dataSize();
        break;

    default:
        qCDebug(Render::Io) << Q_FUNC_INFO << "can't convert" << dataType() << "x" << dataSize() << "to QVector3D";
        return QVector<QVector3D>();
    }

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;
    QVector<QVector3D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector3D v;
        fptr = reinterpret_cast<const float*>(rawBuffer);

        for (uint i = 0, m = qMin(dataSize(), 3U); i < m; ++i)
            v[i] = fptr[i];

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

    switch (dataType()) {
    case QAttribute::Float:
        stride = sizeof(float) * dataSize();
        break;

    default:
        qCDebug(Render::Io) << Q_FUNC_INFO << "can't convert" << dataType() << "x" << dataSize() << "to QVector3D";
        return QVector<QVector2D>();
    }

    if (d->m_byteStride != 0)
        stride = d->m_byteStride;

    QVector<QVector2D> result;
    result.resize(d->m_count);

    for (uint c = 0; c < d->m_count; ++c) {
        QVector2D v;
        fptr = reinterpret_cast<float*>(rawBuffer);
        for (uint i = 0, m = qMin(dataSize(), 2U); i < m; ++i)
            v[i] = fptr[i];
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
        switch (dataType()) {
        case QAttribute::UnsignedShort:
            if (stride == 0)
                stride = sizeof(quint16);
            usptr = reinterpret_cast<const quint16*>(rawBuffer + stride * c);
            qCDebug(Render::Io) << c << ":u16:" << usptr[0];
            break;
        case QAttribute::UnsignedInt:
            if (stride == 0)
                stride = sizeof(quint32);
            qCDebug(Render::Io) << c << ":u32:" << reinterpret_cast<const quint32*>(rawBuffer + stride * c)[0];
            break;
        case QAttribute::Float:
            if (stride == 0)
                stride = sizeof(float) * dataSize();
            fptr = reinterpret_cast<const float*>(rawBuffer + stride * c);
            qCDebug(Render::Io) << c << QString::fromLatin1(":vec") + QString::number(dataSize()) << fptr[0] << fptr[1];
            break;

        default: qCDebug(Render::Io) << Q_FUNC_INFO << "unspported type:" << dataType();
        }
    }
}

QBuffer *QAttribute::buffer() const
{
    return static_cast<QBuffer *>(QAbstractAttribute::buffer());
}

QString QAttribute::defaultPositionAttributeName()
{
    return QStringLiteral("vertexPosition");
}

QString QAttribute::defaultNormalAttributeName()
{
    return QStringLiteral("vertexNormal");
}

QString QAttribute::defaultColorAttributeName()
{
    return QStringLiteral("vertexColor");
}

QString QAttribute::defaultTextureCoordinateAttributeName()
{
    return QStringLiteral("vertexTexCoord");
}

QString QAttribute::defaultTangentAttributeName()
{
    return QStringLiteral("vertexTangent");
}

} // Qt3D

QT_END_NAMESPACE
