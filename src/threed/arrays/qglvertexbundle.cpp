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

#include "qglvertexbundle.h"
#include "qglvertexbundle_p.h"
#include "qglabstracteffect.h"
#include <QtCore/qlist.h>
#include <QtCore/qatomic.h>
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLVertexBundle
    \brief The QGLVertexBundle class bundles vertex attribute arrays for efficient uploading into a GL server.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::arrays

    QGLVertexBundle provides an implementation of a static vertex
    buffer, where the vertex attributes are supplied once at construction
    time and then never modified until the bundle is destroyed.
    When the vertex attributes are sent ot the GL server by upload(),
    they may be repacked for greater drawing efficiency.

    For general-purpose vertex buffers that can be allocated and modified
    in-place, use QGLBuffer instead.
*/

/*!
    Constructs a new vertex bundle.
*/
QGLVertexBundle::QGLVertexBundle()
    : d_ptr(new QGLVertexBundlePrivate())
{
}

/*!
    Creates a copy of \a other.  Note that this just copies a reference
    to the vertex bundle.  Any modifications to the copy will also
    affect the original object.
*/
QGLVertexBundle::QGLVertexBundle(const QGLVertexBundle& other)
    : d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

/*!
    Destroys this vertex bundle if this object is the last reference to it.
*/
QGLVertexBundle::~QGLVertexBundle()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns \a other to this object.  Note that this just assigns a
    reference to the \a other vertex bundle.  Any modifications to this
    object will also affect \a other.
*/
QGLVertexBundle& QGLVertexBundle::operator=(const QGLVertexBundle& other)
{
    if (d_ptr != other.d_ptr) {
        if (!d_ptr->ref.deref())
            delete d_ptr;
        d_ptr = other.d_ptr;
        d_ptr->ref.ref();
    }
    return *this;
}

/*!
    Adds the floating-point array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QArray<float>& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleFloatAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 2D vector array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector2D>& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleVector2DAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 3D vector array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector3D>& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleVector3DAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the 4D vector array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QVector4D>& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleVector4DAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the color array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QArray<QColor4ub>& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleColorAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

/*!
    Adds the custom data array \a value to this vertex bundle as the
    data for \a attribute.

    \sa upload()
*/
void QGLVertexBundle::addAttribute
    (QGL::VertexAttribute attribute, const QCustomDataArray& value)
{
    Q_D(QGLVertexBundle);
    if (!d->buffer.isCreated()) {
        d->attributeSet.insert(attribute);
        d->attributes +=
            new QGLVertexBundleCustomAttribute(attribute, value);
        d->vertexCount = qMax(d->vertexCount, value.count());
    }
}

// Interleave a source array into a destination array.
static void vertexBufferInterleave
    (float *dst, int dstStride, const float *src, int srcStride, int count)
{
    switch (srcStride) {
    case 1:
        while (count-- > 0) {
            dst[0] = src[0];
            ++src;
            dst += dstStride;
        }
        break;
    case 2:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            src += 2;
            dst += dstStride;
        }
        break;
    case 3:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            src += 3;
            dst += dstStride;
        }
        break;
    case 4:
        while (count-- > 0) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            src += 4;
            dst += dstStride;
        }
        break;
    default:
        while (count-- > 0) {
            for (int component = 0; component < srcStride; ++component)
                dst[component] = src[component];
            src += srcStride;
            dst += dstStride;
        }
        break;
    }
}

/*!
    Returns the set of attributes that are present in this vertex bundle.
*/
QGLAttributeSet QGLVertexBundle::attributes() const
{
    Q_D(const QGLVertexBundle);
    return d->attributeSet;
}

/*!
    Returns the raw attribute value associated with \a attribute in
    this vertex bundle; null if \a attribute does not exist in the
    vertex bundle.

    If isUploaded() is true, then the returned value will contain a
    buffer offset to the attribute.  If isUploaded() is false,
    then the returned value will contain a client-side data pointer
    to the attribute.

    \sa addAttribute()
*/
QGLAttributeValue QGLVertexBundle::attributeValue(QGL::VertexAttribute attribute) const
{
    Q_D(const QGLVertexBundle);
    QGLVertexBundleAttribute *attr = 0;
    int attrIndex;
    for (attrIndex = 0; attrIndex < d->attributes.size(); ++attrIndex) {
        attr = d->attributes[attrIndex];
        if (attr->attribute == attribute)
            return attr->value;
    }
    return QGLAttributeValue();
}

/*!
    Returns the number of vertices that were defined by previous
    called to addAttribute().

    \sa addAttribute()
*/
int QGLVertexBundle::vertexCount() const
{
    Q_D(const QGLVertexBundle);
    return d->vertexCount;
}

/*!
    \fn bool QGLVertexBundle::isEmpty() const

    Returns true if vertexCount() is zero; false otherwise.
*/

/*!
    Uploads the vertex data specified by previous addAttribute()
    calls into the GL server as a vertex buffer object.

    Returns true if the data could be uploaded; false if vertex buffer
    objects are not supported or there is insufficient memory to complete
    the request.  Returns true if the data was already uploaded.

    Once the vertex data has been uploaded, the client-side copies of
    the data arrays will be released.  If the vertex data could not be
    uploaded, then it is retained client-side.  This way, regardless of
    whether the data could be uploaded or not, QGLPainter::setVertexBundle()
    can be used to support drawing of primitives using this object.

    \sa isUploaded(), addAttribute(), QGLPainter::setVertexBundle()
*/
bool QGLVertexBundle::upload()
{
    Q_D(QGLVertexBundle);
    QGLVertexBundleAttribute *attr;

    // Nothing to do if already uploaded or there are no attributes.
    if (d->buffer.isCreated())
        return true;
    if (d->attributes.isEmpty())
        return false;

    // Create the VBO in the GL server and bind it.
    if (!d->buffer.create())
        return false;
    d->buffer.bind();

    // If there is only one attribute, then realloc and write in one step.
    if (d->attributes.size() == 1) {
        attr = d->attributes[0];
        d->buffer.allocate(attr->value.data(),
                           attr->count() * attr->elementSize());
        attr->value.setOffset(0);
        attr->clear();
        d->buffer.release();
        return true;
    }

    // Calculate the total size of the VBO that we will need,
    // the maximum number of interleaved vertices, and the
    // interleaved stride.
    int size = 0;
    int stride = 0;
    int maxCount = 0;
    for (int index = 0; index < d->attributes.size(); ++index) {
        attr = d->attributes[index];
        int count = attr->count();
        if (count > maxCount)
            maxCount = count;
        int elemSize = attr->elementSize();
        size += count * elemSize;
        stride += elemSize;
    }
    int bufferSize = size;
    d->buffer.allocate(bufferSize);
    stride /= sizeof(float);

    // Determine how to upload the data, using a map if possible.
    // Interleave the data into the final buffer.  We do it in
    // sections so as to keep locality problems to a minimum.
    void *mapped = d->buffer.map(QGLBuffer::WriteOnly);
    int offset = 0;
    QArray<float> temp;
    float *dst;
    if (mapped)
        dst = reinterpret_cast<float *>(mapped);
    else
        dst = temp.extend(1024);
    int sectionSize = 1024 / stride;
    for (int vertex = 0; vertex < maxCount; vertex += sectionSize) {
        int attrPosn = 0;
        for (int index = 0; index < d->attributes.size(); ++index) {
            attr = d->attributes[index];
            int count = attr->count() - vertex;
            if (count <= 0)
                continue;
            count = qMin(count, sectionSize);
            int components = attr->elementSize() / sizeof(float);
            vertexBufferInterleave
                (dst + attrPosn, stride,
                 reinterpret_cast<const float *>(attr->value.data()) +
                        vertex * components,
                 components, count);
            attrPosn += attr->elementSize() / sizeof(float);
        }
        size = sectionSize * stride;
        if (mapped) {
            dst += size;
        } else {
            size *= sizeof(float);
            if ((offset + size) > bufferSize)    // buffer overflow check
                size = bufferSize-offset;
            d->buffer.write(offset, dst, size);
            offset += size;
        }
    }
    offset = 0;
    for (int index = 0; index < d->attributes.size(); ++index) {
        attr = d->attributes[index];
        attr->value.setOffset(offset);
        attr->value.setStride(stride * sizeof(float));
        offset += attr->elementSize();
        attr->clear();
    }
    if (mapped)
        d->buffer.unmap();

    // Buffer is uploaded and ready to go.
    d->buffer.release();
    return true;
}

/*!
    Returns true if the vertex data specified by previous addAttribute()
    calls has been uploaded into the GL server; false otherwise.

    \sa upload(), addAttribute()
*/
bool QGLVertexBundle::isUploaded() const
{
    Q_D(const QGLVertexBundle);
    return d->buffer.isCreated();
}

/*!
    Returns the QGLBuffer in use by this vertex bundle object,
    so that its properties or contents can be modified directly.

    \sa isUploaded()
*/
QGLBuffer QGLVertexBundle::buffer() const
{
    Q_D(const QGLVertexBundle);
    return d->buffer;
}

/*!
    Binds the vertex buffer associated with this bundle to the current GL
    context.  Returns false if binding was not possible, usually because
    upload() has not been called.

    The buffer must be bound to the same QGLContext current when upload()
    was called, or to another QGLContext that is sharing with it.
    Otherwise, false will be returned from this function.

    \sa release(), upload()
*/
bool QGLVertexBundle::bind()
{
    Q_D(QGLVertexBundle);
    return d->buffer.bind();
}

/*!
    Releases the vertex buffer associated with this bundle from the
    current GL context.

    This function must be called with the same QGLContext current
    as when bind() was called on the vertex buffer.

    \sa bind()
*/
void QGLVertexBundle::release()
{
    Q_D(QGLVertexBundle);
    d->buffer.release();
}

QT_END_NAMESPACE
