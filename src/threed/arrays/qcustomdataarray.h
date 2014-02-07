/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QCUSTOMDATAARRAY_H
#define QCUSTOMDATAARRAY_H

#include <Qt3D/qarray.h>
#include <Qt3D/qcolor4ub.h>

#include <QtCore/qvariant.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

QT_BEGIN_NAMESPACE

class QGLVertexBundleCustomAttribute;
class QGeometryData;

class Q_QT3D_EXPORT QCustomDataArray
{
public:
    enum ElementType
    {
        Float,
        Vector2D,
        Vector3D,
        Vector4D,
        Color
    };

    QCustomDataArray();
    explicit QCustomDataArray(QCustomDataArray::ElementType type);
    QCustomDataArray(QCustomDataArray::ElementType type, int size);
    QCustomDataArray(const QCustomDataArray& other);
    QCustomDataArray(const QArray<float>& other);
    QCustomDataArray(const QArray<QVector2D>& other);
    QCustomDataArray(const QArray<QVector3D>& other);
    QCustomDataArray(const QArray<QVector4D>& other);
    QCustomDataArray(const QArray<QColor4ub>& other);
    ~QCustomDataArray();

    QCustomDataArray& operator=(const QCustomDataArray& other);

    QCustomDataArray::ElementType elementType() const;
    void setElementType(QCustomDataArray::ElementType type);

    int size() const;
    int count() const;

    int capacity() const;
    bool isEmpty() const;

    int elementSize() const;

    void clear();
    void reserve(int newCapacity);
    void resize(int newSize);
    void squeeze();

    QVariant at(int index) const;
    void setAt(int index, const QVariant& value);

    void setAt(int index, float x);
    void setAt(int index, float x, float y);
    void setAt(int index, float x, float y, float z);
    void setAt(int index, float x, float y, float z, float w);
    void setAt(int index, const QVector2D& value);
    void setAt(int index, const QVector3D& value);
    void setAt(int index, const QVector4D& value);
    void setAt(int index, const QColor4ub& value);
    void setAt(int index, Qt::GlobalColor value);

    float floatAt(int index) const;
    QVector2D vector2DAt(int index) const;
    QVector3D vector3DAt(int index) const;
    QVector4D vector4DAt(int index) const;
    QColor4ub colorAt(int index) const;

    void append(float x);
    void append(float x, float y);
    void append(float x, float y, float z);
    void append(float x, float y, float z, float w);
    void append(const QVector2D& value);
    void append(const QVector3D& value);
    void append(const QVector4D& value);
    void append(const QColor4ub& value);
    void append(const QVariant& value);
    void append(Qt::GlobalColor value);
    void append(const QCustomDataArray &array);

    QArray<float> toFloatArray() const;
    QArray<QVector2D> toVector2DArray() const;
    QArray<QVector3D> toVector3DArray() const;
    QArray<QVector4D> toVector4DArray() const;
    QArray<QColor4ub> toColorArray() const;

    const void *data() const;

private:
    QArray<float> m_array;
    QCustomDataArray::ElementType m_elementType;
    int m_elementComponents;

    friend class QGLVertexBundleCustomAttribute;
    friend class QGeometryData;
};

inline QCustomDataArray::QCustomDataArray()
    : m_elementType(QCustomDataArray::Float),
      m_elementComponents(1)
{
}

inline QCustomDataArray::QCustomDataArray(const QCustomDataArray& other)
    : m_array(other.m_array),
      m_elementType(other.m_elementType),
      m_elementComponents(other.m_elementComponents)
{
}

inline QCustomDataArray::~QCustomDataArray() {}

inline QCustomDataArray& QCustomDataArray::operator=(const QCustomDataArray& other)
{
    if (this != &other) {
        m_array = other.m_array;
        m_elementType = other.m_elementType;
        m_elementComponents = other.m_elementComponents;
    }
    return *this;
}

inline QCustomDataArray::ElementType QCustomDataArray::elementType() const
{
    return m_elementType;
}

inline int QCustomDataArray::size() const
{
    return m_array.size() / m_elementComponents;
}

inline int QCustomDataArray::count() const
{
    return m_array.size() / m_elementComponents;
}

inline int QCustomDataArray::capacity() const
{
    return m_array.capacity() / m_elementComponents;
}

inline bool QCustomDataArray::isEmpty() const
{
    return m_array.isEmpty();
}

inline int QCustomDataArray::elementSize() const
{
    return m_elementComponents * sizeof(float);
}

inline void QCustomDataArray::clear()
{
    m_array.clear();
}

inline void QCustomDataArray::reserve(int newCapacity)
{
    m_array.reserve(newCapacity * m_elementComponents);
}

inline void QCustomDataArray::resize(int newSize)
{
    m_array.resize(newSize * m_elementComponents);
}

inline void QCustomDataArray::squeeze()
{
    m_array.squeeze();
}

inline void QCustomDataArray::setAt(int index, float x)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Float);
    Q_ASSERT(index >= 0 && index < size());
    m_array[index] = x;
}

inline void QCustomDataArray::setAt(int index, float x, float y)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 2;
    dataptr[0] = x;
    dataptr[1] = y;
}

inline void QCustomDataArray::setAt(int index, float x, float y, float z)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 3;
    dataptr[0] = x;
    dataptr[1] = y;
    dataptr[2] = z;
}

inline void QCustomDataArray::setAt(int index, float x, float y, float z, float w)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 4;
    dataptr[0] = x;
    dataptr[1] = y;
    dataptr[2] = z;
    dataptr[3] = w;
}

inline void QCustomDataArray::setAt(int index, const QVector2D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 2;
    dataptr[0] = value.x();
    dataptr[1] = value.y();
}

inline void QCustomDataArray::setAt(int index, const QVector3D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 3;
    dataptr[0] = value.x();
    dataptr[1] = value.y();
    dataptr[2] = value.z();
}

inline void QCustomDataArray::setAt(int index, const QVector4D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    Q_ASSERT(index >= 0 && index < size());
    float *dataptr = m_array.data() + index * 4;
    dataptr[0] = value.x();
    dataptr[1] = value.y();
    dataptr[2] = value.z();
    dataptr[3] = value.w();
}

inline void QCustomDataArray::setAt(int index, const QColor4ub& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    Q_ASSERT(index >= 0 && index < size());
    *(reinterpret_cast<QColor4ub *>(m_array.data() + index)) = value;
}

inline void QCustomDataArray::setAt(int index, Qt::GlobalColor value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    Q_ASSERT(index >= 0 && index < size());
    *(reinterpret_cast<QColor4ub *>(m_array.data() + index)) = QColor4ub(value);
}

inline float QCustomDataArray::floatAt(int index) const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Float);
    Q_ASSERT(index >= 0 && index < size());
    return m_array.at(index);
}

inline QVector2D QCustomDataArray::vector2DAt(int index) const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    Q_ASSERT(index >= 0 && index < size());
    const float *dataptr = m_array.constData() + index * 2;
    return QVector2D(dataptr[0], dataptr[1]);
}

inline QVector3D QCustomDataArray::vector3DAt(int index) const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    Q_ASSERT(index >= 0 && index < size());
    const float *dataptr = m_array.constData() + index * 3;
    return QVector3D(dataptr[0], dataptr[1], dataptr[2]);
}

inline QVector4D QCustomDataArray::vector4DAt(int index) const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    Q_ASSERT(index >= 0 && index < size());
    const float *dataptr = m_array.constData() + index * 4;
    return QVector4D(dataptr[0], dataptr[1], dataptr[2], dataptr[3]);
}

inline QColor4ub QCustomDataArray::colorAt(int index) const
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    Q_ASSERT(index >= 0 && index < size());
    return *(reinterpret_cast<const QColor4ub *>(m_array.constData() + index));
}

inline void QCustomDataArray::append(float x)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Float);
    m_array.append(x);
}

inline void QCustomDataArray::append(float x, float y)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    m_array.append(x, y);
}

inline void QCustomDataArray::append(float x, float y, float z)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    m_array.append(x, y, z);
}

inline void QCustomDataArray::append(float x, float y, float z, float w)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    m_array.append(x, y, z, w);
}

inline void QCustomDataArray::append(const QVector2D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector2D);
    m_array.append(value.x(), value.y());
}

inline void QCustomDataArray::append(const QVector3D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector3D);
    m_array.append(value.x(), value.y(), value.z());
}

inline void QCustomDataArray::append(const QVector4D& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Vector4D);
    m_array.append(value.x(), value.y(),
                   value.z(), value.w());
}

inline void QCustomDataArray::append(const QColor4ub& value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    *(reinterpret_cast<QColor4ub *>(m_array.extend(1))) = value;
}

inline void QCustomDataArray::append(Qt::GlobalColor value)
{
    Q_ASSERT(m_elementType == QCustomDataArray::Color);
    *(reinterpret_cast<QColor4ub *>(m_array.extend(1))) = QColor4ub(value);
}

inline void QCustomDataArray::append(const QCustomDataArray &array)
{
    Q_ASSERT(isEmpty() || (array.elementType() == elementType()));
    if (isEmpty())
        *this = array;
    else
        m_array.append(array.m_array);
}

inline const void *QCustomDataArray::data() const
{
    return m_array.constData();
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QCustomDataArray &array);
#endif

QT_END_NAMESPACE

#endif
