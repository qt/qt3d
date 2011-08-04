/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLATTRIBUTEVALUE_H
#define QGLATTRIBUTEVALUE_H

#include "qglattributedescription.h"
#include "qcustomdataarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLVertexBundle;

class Q_QT3D_EXPORT QGLAttributeValue
{
public:
    QGLAttributeValue();
    QGLAttributeValue(const QArray<float>& array);
    QGLAttributeValue(const QArray<QVector2D>& array);
    QGLAttributeValue(const QArray<QVector3D>& array);
    QGLAttributeValue(const QArray<QVector4D>& array);
    QGLAttributeValue(const QArray<QColor4ub>& array);
    QGLAttributeValue(const QCustomDataArray& array);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count = 0);
    QGLAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count = 0);
    QGLAttributeValue(const QGLAttributeDescription& description, const void *data, int count = 0);
    QGLAttributeValue(const QGLAttributeDescription& description, int offset, int count = 0);

    bool isNull() const;

    QGLAttributeDescription description(QGL::VertexAttribute attribute) const;
    GLenum type() const;
    int sizeOfType() const;
    int tupleSize() const;
    int stride() const;
    const void *data() const;
    int count() const;

private:
    int m_tupleSize;
    GLenum m_type;
    int m_stride;
    const void *m_data;
    int m_count;

    void setStride(int stride) { m_stride = stride; }
    void setOffset(int offset)
        { m_data = reinterpret_cast<const void *>(offset); }

    friend class QGLVertexBundle;
};

inline QGLAttributeValue::QGLAttributeValue()
    : m_tupleSize(0), m_type(GL_FLOAT), m_stride(0), m_data(0), m_count(0)
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<float>& array)
    : m_tupleSize(1), m_type(GL_FLOAT), m_stride(0)
    , m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector2D>& array)
    : m_tupleSize(2), m_type(GL_FLOAT), m_stride(0)
    , m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector3D>& array)
    : m_tupleSize(3), m_type(GL_FLOAT), m_stride(0)
    , m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QVector4D>& array)
    : m_tupleSize(4), m_type(GL_FLOAT), m_stride(0)
    , m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue(const QArray<QColor4ub>& array)
    : m_tupleSize(4), m_type(GL_UNSIGNED_BYTE), m_stride(0)
    , m_data(array.constData()), m_count(array.count())
{
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, const void *data, int count)
    : m_tupleSize(tupleSize), m_type(type), m_stride(stride)
    , m_data(data), m_count(count)
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
}

inline QGLAttributeValue::QGLAttributeValue
        (int tupleSize, GLenum type, int stride, int offset, int count)
    : m_tupleSize(tupleSize), m_type(type), m_stride(stride)
    , m_data(reinterpret_cast<const void *>(offset)), m_count(count)
{
    Q_ASSERT(tupleSize >= 1 && tupleSize <= 4);
}

inline QGLAttributeValue::QGLAttributeValue
        (const QGLAttributeDescription& description, const void *data, int count)
    : m_tupleSize(description.tupleSize()), m_type(description.type())
    , m_stride(description.stride()), m_data(data), m_count(count)
{
}

inline QGLAttributeValue::QGLAttributeValue
        (const QGLAttributeDescription& description, int offset, int count)
    : m_tupleSize(description.tupleSize()), m_type(description.type())
    , m_stride(description.stride())
    , m_data(reinterpret_cast<const void *>(offset))
    , m_count(count)
{
}

inline bool QGLAttributeValue::isNull() const
{
    return m_tupleSize == 0;
}

inline QGLAttributeDescription QGLAttributeValue::description(QGL::VertexAttribute attribute) const
{
    if (!isNull()) {
        return QGLAttributeDescription(attribute, m_tupleSize, m_type, m_stride);
    } else {
        QGLAttributeDescription desc;
        desc.setAttribute(attribute);
        return desc;
    }
}

inline GLenum QGLAttributeValue::type() const
{
    return m_type;
}

inline int QGLAttributeValue::tupleSize() const
{
    return m_tupleSize;
}

inline int QGLAttributeValue::stride() const
{
    return m_stride;
}

inline const void *QGLAttributeValue::data() const
{
    return m_data;
}

inline int QGLAttributeValue::count() const
{
    return m_count;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
