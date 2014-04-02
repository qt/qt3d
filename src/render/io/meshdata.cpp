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

#include "meshdata.h"

#include <QSet>
#include <QDebug>
#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE

namespace Qt3D {

GLint elementType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
        return GL_FLOAT;

    case GL_DOUBLE:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
#endif
        return GL_DOUBLE;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return GL_INVALID_VALUE;
}

GLint tupleSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
    case GL_DOUBLE:
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_INT:
        break; // fall through

    case GL_FLOAT_VEC2:
#ifdef GL_DOUBLE_VEC2 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC2:
#endif
        return 2;

    case GL_FLOAT_VEC3:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC3:
#endif
        return 3;

    case GL_FLOAT_VEC4:
#ifdef GL_DOUBLE_VEC4 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC4:
#endif
        return 4;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 1;
}

GLuint byteSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:          return sizeof(float);
    case GL_DOUBLE:         return sizeof(double);
    case GL_UNSIGNED_BYTE:  return sizeof(unsigned char);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);

    case GL_FLOAT_VEC2:     return sizeof(float) * 2;
    case GL_FLOAT_VEC3:     return sizeof(float) * 3;
    case GL_FLOAT_VEC4:     return sizeof(float) * 4;
#ifdef GL_DOUBLE_VEC3 // Required to compile on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:    return sizeof(double) * 2;
    case GL_DOUBLE_VEC3:    return sizeof(double) * 3;
    case GL_DOUBLE_VEC4:    return sizeof(double) * 4;
#endif
    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 0;
}

MeshData::MeshData()
    : m_primitiveType(0)
{
}

MeshData::MeshData(int primitiveType) :
    m_primitiveType(primitiveType)
{
    Q_ASSERT((m_primitiveType == GL_TRIANGLES) ||
             (m_primitiveType == GL_LINES) ||
             (m_primitiveType == GL_POINTS));
}

void MeshData::addAttribute(QString name, AttributePtr attr)
{
    Q_ASSERT(!m_attributes.contains(name));
    m_attributes[name] = attr;
}

void MeshData::addAttribute(QString name, Attribute *attr)
{
    Q_ASSERT(!m_attributes.contains(name));
    m_attributes[name] = AttributePtr(attr);
}

void MeshData::setIndexAttr(AttributePtr indexAttr)
{
    m_indexAttr = indexAttr;
}

void MeshData::setIndexData(BufferPtr buf, int type, int count, int offset)
{
    m_indexAttr = AttributePtr(new Attribute(buf, type, count, offset));
}

GLint MeshData::primitiveType() const
{
    return m_primitiveType;
}

GLsizei MeshData::primitiveCount() const
{
    if (m_indexAttr) {
        return m_indexAttr->count();
    } else {
        // assume all attribute arrays have the same size
        // will break with instanced drawing, but probably per-instance
        // arrays aren't coming from this code-path.
        // Maybe.
        return m_attributes.first()->count();
    }
}

QStringList MeshData::attributeNames() const
{
    return m_attributes.keys();
}

AttributePtr MeshData::attributeByName(QString nm) const
{
    return m_attributes.value(nm);
}

AttributePtr MeshData::indexAttr() const
{
    return m_indexAttr;
}

QList<BufferPtr> MeshData::buffers() const
{
    QSet<BufferPtr> r;
    if (m_indexAttr)
        r.insert(m_indexAttr->buffer());

    foreach (AttributePtr v, m_attributes.values())
        r.insert(v->buffer());

    return r.toList();
}

void MeshData::setBoundingBox(const AxisAlignedBoundingBox &bbox)
{
    m_box = bbox;
}

void MeshData::computeBoundsFromAttribute(QString name)
{
    AttributePtr attr = attributeByName(name);
    if (!attr) {
        qWarning() << Q_FUNC_INFO << "unknoen attribute:" << name;
        return;
    }

    m_box.clear();
    m_box.update(attr->asVector3D());
}

Attribute::Attribute(BufferPtr buf, int type, int count, int offset, int stride) :
    m_buffer(buf),
    m_type(type),
    m_count(count),
    m_stride(stride),
    m_offset(offset),
    m_divisor(0)
{

}

void Attribute::setDivisor(unsigned int divisor)
{
    m_divisor = divisor;
}

BufferPtr Attribute::buffer() const
{
    return m_buffer;
}

QVector<QVector3D> Attribute::asVector3D() const
{
    const char* rawBuffer = m_buffer->data().constData();
    rawBuffer += m_offset;
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
        qDebug() << Q_FUNC_INFO << "can't convert" << QString::number(type(), 16) << "to QVector3D";
        return QVector<QVector3D>();
    }

    if (m_stride != 0)
        stride = m_stride;
    QVector<QVector3D> result;
    result.resize(m_count);

    for (int c=0; c<m_count; ++c) {
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
    char* rawBuffer = m_buffer->data().data();
    rawBuffer += m_offset;
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
        qDebug() << Q_FUNC_INFO << "can't convert" << QString::number(type(), 16) << "to QVector2D";
        return QVector<QVector2D>();
    }

    if (m_stride != 0)
        stride = m_stride;

    QVector<QVector2D> result;
    result.resize(m_count);

    for (int c=0; c<m_count; ++c) {
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
    char* rawBuffer = m_buffer->data().data();
    rawBuffer += m_offset;

    float* fptr;
    quint16* usptr;

    int stride = m_stride;

    for (int c=0; c<count; ++c) {
        switch (type()) {
        case GL_UNSIGNED_SHORT:
            if (!stride) stride = sizeof(quint16);
            usptr = reinterpret_cast<quint16*>(rawBuffer);
            qDebug() << c << ":u16:" << usptr[0];
            break;
        case GL_UNSIGNED_INT:
            if (!stride) stride = sizeof(quint32);
            qDebug() << c << ":u32:" << reinterpret_cast<quint32*>(rawBuffer)[0];
            break;
        case GL_FLOAT_VEC2:
            if (!stride) stride = sizeof(float) * 2;
            fptr = reinterpret_cast<float*>(rawBuffer);
            qDebug() << c << ":vec2:"<< fptr[0] << fptr[0];
            break;

        case GL_FLOAT_VEC3:
            if (!stride) stride = sizeof(float) * 3;
            fptr = reinterpret_cast<float*>(rawBuffer);
            qDebug() << c << ":vec3:" << fptr[0] << fptr[0] << fptr[2];
            break;

        default: qDebug() << Q_FUNC_INFO << "unspported type:" << QString::number(type(), 16);
        }


    }
}

Buffer::Buffer(QOpenGLBuffer::Type ty) :
    m_type(ty),
    m_usage(QOpenGLBuffer::StaticDraw)
{
}

void Buffer::setUsage(QOpenGLBuffer::UsagePattern usage)
{
    m_usage = usage;
}

void Buffer::setData(QByteArray bytes)
{
    m_clientSideBytes = bytes;
    // mark as dirty for dynamic / stream data
    // if static, check this is the first and only set.
}

QByteArray Buffer::data() const
{
    return m_clientSideBytes;
}

QOpenGLBuffer Buffer::createGL() const
{
    QOpenGLBuffer b(m_type);
    b.setUsagePattern(m_usage);
    if (!b.create())
        qWarning() << Q_FUNC_INFO << "buffer creation failed";

    if (!b.bind())
        qWarning() << Q_FUNC_INFO << "buffer binding failed";

    b.allocate(m_clientSideBytes.count());
    b.release();
    return b;
}

void Buffer::upload(QOpenGLBuffer b)
{
    if (!b.bind())
        qWarning() << Q_FUNC_INFO << "buffer bind failed";
    b.allocate(NULL, m_clientSideBytes.count()); // orphan the buffer
    b.allocate(m_clientSideBytes.data(),
               m_clientSideBytes.count());
    b.release();
    qDebug() << "uploaded buffer size=" << m_clientSideBytes.count();
}

} // of namespace

QT_END_NAMESPACE
