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

#include "qglindexbuffer.h"
#include "qglpainter.h"
#include "qglpainter_p.h"
#include "qglext_p.h"
#include <QtOpenGL/qgl.h>
#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLIndexBuffer
    \brief The QGLIndexBuffer class manages uploading of index arrays into a GL server.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::arrays
*/

#ifdef QT_OPENGL_ES

static bool qt_has_uint_buffers()
{
    static bool done = false;
    static bool answer = false;
    if (!done) {
        QGLExtensionChecker extensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        answer = extensions.match("GL_OES_element_index_uint");
        done = true;
    }
    return answer;
}

#endif

class QGLIndexBufferPrivate
{
public:
    QGLIndexBufferPrivate()
        : indexCount(0)
        , elementType(GL_UNSIGNED_SHORT)
        , buffer(QGLBuffer::IndexBuffer)
#ifdef QT_OPENGL_ES
        , hasIntBuffers(qt_has_uint_buffers())
#else
        , hasIntBuffers(true)
#endif
    {
        ref = 1;
    }

    QBasicAtomicInt ref;
    int indexCount;
    QArray<ushort> indexesShort;
    QArray<uint> indexesInt;
    GLenum elementType;
    QGLBuffer buffer;
    bool hasIntBuffers;

    void append(const QGLIndexBufferPrivate *other, uint offset, int start);
    uint headIndex(int posn) const;
    uint tailIndex(int posn) const;
};

/*!
    Creates a new index buffer.
*/
QGLIndexBuffer::QGLIndexBuffer()
    : d_ptr(new QGLIndexBufferPrivate)
{
}

/*!
    Creates a copy of \a other.  Note that this just copies a reference
    to the index buffer.  Any modifications to the copy will also
    affect the original object.
*/
QGLIndexBuffer::QGLIndexBuffer(const QGLIndexBuffer& other)
    : d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

/*!
    Destroys this index buffer if this object is the last reference to it.
*/
QGLIndexBuffer::~QGLIndexBuffer()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns \a other to this object.  Note that this just assigns a
    reference to the \a other index buffer.  Any modifications to this
    object will also affect \a other.
*/
QGLIndexBuffer& QGLIndexBuffer::operator=(const QGLIndexBuffer& other)
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
    Returns the indexes in this buffer as an array of ushort values.

    Returns an empty array if type() is not \c GL_UNSIGNED_SHORT or the
    buffer has already been uploaded.
*/
QArray<ushort> QGLIndexBuffer::indexesUShort() const
{
    Q_D(const QGLIndexBuffer);
    return d->indexesShort;
}

/*!
    Returns the indexes in this buffer as an array of uint values.

    Returns an empty array if type() is not \c GL_UNSIGNED_INT or the
    buffer has already been uploaded.
*/
QArray<uint> QGLIndexBuffer::indexesUInt() const
{
    Q_D(const QGLIndexBuffer);
    return d->indexesInt;
}

/*!
    Returns the usage pattern for this index buffer.
    The default value is QGLBuffer::StaticDraw.

    \sa setUsagePattern()
*/
QGLBuffer::UsagePattern QGLIndexBuffer::usagePattern() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer.usagePattern();
}

/*!
    Sets the usage pattern for this index buffer to \a value.
    This function must be called before upload() for the \a value
    to take effect.

    \sa usagePattern(), upload()
*/
void QGLIndexBuffer::setUsagePattern(QGLBuffer::UsagePattern value)
{
    Q_D(QGLIndexBuffer);
    d->buffer.setUsagePattern(value);
}

static QArray<ushort> qt_qarray_uint_to_ushort(const QArray<uint> &array)
{
    QArray<ushort> result;
    const uint *values = array.constData();
    int size = array.size();
    bool largeValue = false;
    result.reserve(size);
    while (size-- > 0) {
        uint value = *values++;
        if (ushort(value) != value)
            largeValue = true;
        result.append(ushort(value));
    }
    if (largeValue)
        qWarning("QGLIndexBuffer::setIndexes: large 32-bit value provided to a 16-bit only buffer");
    return result;
}

/*!
    Sets the index \a values in this index buffer, replacing the
    entire current contents.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    \sa replaceIndexes()
*/
void QGLIndexBuffer::setIndexes(const QArray<ushort>& values)
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.allocate(values.constData(), values.size() * sizeof(ushort));
        d->buffer.release();
        // The element type may have changed from int to ushort.
        d->elementType = GL_UNSIGNED_SHORT;
    } else {
        d->indexesShort = values;
        d->elementType = GL_UNSIGNED_SHORT;
        d->indexesInt = QArray<uint>();
    }
    d->indexCount = values.size();
}

/*!
    Sets the index \a values in this index buffer, replacing the
    entire current contents.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    OpenGL/ES systems usually do not support 32-bit index values unless
    they have a special extension for that purpose.  On systems without
    32-bit index values, this function will need to convert all values
    to 16-bit which may incur a performance penalty and lose information.

    \sa replaceIndexes()
*/
void QGLIndexBuffer::setIndexes(const QArray<uint>& values)
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated()) {
        if (d->hasIntBuffers) {
            d->buffer.bind();
            d->buffer.allocate(values.constData(), values.size() * sizeof(int));
            d->buffer.release();
            // The element type may have changed from ushort to int.
            d->elementType = GL_UNSIGNED_INT;
        } else {
            QArray<ushort> svalues = qt_qarray_uint_to_ushort(values);
            d->buffer.bind();
            d->buffer.allocate(svalues.constData(), svalues.size() * sizeof(ushort));
            d->buffer.release();
        }
    } else if (d->hasIntBuffers) {
        d->indexesInt = values;
        d->elementType = GL_UNSIGNED_INT;
        d->indexesShort = QArray<ushort>();
    } else {
        d->indexesShort = qt_qarray_uint_to_ushort(values);
        d->elementType = GL_UNSIGNED_SHORT;
        d->indexesInt = QArray<uint>();
    }
    d->indexCount = values.size();
}

/*!
    Replaces the elements of this index buffer, starting at \a index,
    with the contents of \a values.  All other elements keep their
    current values.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    The index buffer must have been originally created with the
    ushort element type.

    \sa setIndexes()
*/
void QGLIndexBuffer::replaceIndexes(int index, const QArray<ushort>& values)
{
    Q_D(QGLIndexBuffer);
    Q_ASSERT_X(d->elementType == GL_UNSIGNED_SHORT,
               "QGLIndexBuffer::replaceIndexes()",
               "buffer created with int element type, replacing with ushort");
    if (d->elementType != GL_UNSIGNED_SHORT)
        return;
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.write(index * sizeof(ushort),
                        values.constData(), values.size() * sizeof(ushort));
        d->buffer.release();
    } else {
        d->indexesShort.replace(index, values.constData(), values.size());
        d->indexCount = d->indexesShort.size();
    }
}

/*!
    Replaces the elements of this index buffer, starting at \a index,
    with the contents of \a values.  All other elements keep their
    current values.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    The index buffer must have been originally created with the
    int element type.

    OpenGL/ES systems usually do not support 32-bit index values unless
    they have a special extension for that purpose.  On systems without
    32-bit index values, this function will need to convert all values
    to 16-bit which may incur a performance penalty and lose information.

    \sa setIndexes()
*/
void QGLIndexBuffer::replaceIndexes(int index, const QArray<uint>& values)
{
    Q_D(QGLIndexBuffer);
    Q_ASSERT_X(d->elementType == GL_UNSIGNED_INT || !d->hasIntBuffers,
               "QGLIndexBuffer::replaceIndexes()",
               "buffer created with ushort element type, replacing with int");
    if (d->elementType != GL_UNSIGNED_INT && d->hasIntBuffers)
        return;
    if (d->buffer.isCreated()) {
        if (d->hasIntBuffers) {
            d->buffer.bind();
            d->buffer.write(index * sizeof(int),
                            values.constData(), values.size() * sizeof(int));
            d->buffer.release();
        } else {
            QArray<ushort> svalues = qt_qarray_uint_to_ushort(values);
            d->buffer.bind();
            d->buffer.write(index * sizeof(ushort),
                            svalues.constData(),
                            svalues.size() * sizeof(ushort));
            d->buffer.release();
        }
    } else if (d->elementType == GL_UNSIGNED_INT) {
        d->indexesInt.replace(index, values.constData(), values.size());
        d->indexCount = d->indexesInt.size();
    } else {
        QArray<ushort> svalues = qt_qarray_uint_to_ushort(values);
        d->indexesShort.replace(index, svalues.constData(), svalues.size());
        d->indexCount = d->indexesShort.size();
    }
}

/*!
    Returns the element type for this index buffer, \c{GL_UNSIGNED_SHORT}
    or \c{GL_UNSIGNED_INT}.
*/
GLenum QGLIndexBuffer::elementType() const
{
    Q_D(const QGLIndexBuffer);
    return d->elementType;
}

/*!
    Returns the number of indexes in this index buffer.
*/
int QGLIndexBuffer::indexCount() const
{
    Q_D(const QGLIndexBuffer);
    return d->indexCount;
}

/*!
    \fn bool QGLIndexBuffer::isEmpty() const

    Returns true if indexCount() is zero; false otherwise.
*/

/*!
    Uploads the index data specified by a previous setIndexes()
    call into the GL server as an index buffer object.

    Returns true if the data could be uploaded; false if index buffer
    objects are not supported or there is insufficient memory to complete
    the request.  Returns true if the data was already uploaded.

    Once the index data has been uploaded, the client-side copies of
    the data arrays will be released.  If the index data could not be
    uploaded, then it is retained client-side.  This way, regardless of
    whether the data could be uploaded or not, QGLPainter::draw() can
    be used to support drawing of primitives using this object.

    \sa isUploaded(), setIndexes(), QGLPainter::draw()
*/
bool QGLIndexBuffer::upload()
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated())
        return true;
    if (!d->buffer.create())
        return false;
    d->buffer.bind();
    if (d->elementType == GL_UNSIGNED_SHORT) {
        d->buffer.allocate(d->indexesShort.constData(),
                           d->indexesShort.size() * sizeof(ushort));
        d->indexesShort = QArray<ushort>();
    } else {
        d->buffer.allocate(d->indexesInt.constData(),
                           d->indexesInt.size() * sizeof(int));
        d->indexesInt = QArray<uint>();
    }
    d->buffer.release();
    return true;
}

/*!
    Returns true if the index data specified by previous a setIndexes()
    call has been uploaded into the GL server; false otherwise.

    \sa upload(), setIndexes()
*/
bool QGLIndexBuffer::isUploaded() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer.isCreated();
}

/*!
    Returns the QGLBuffer in use by this index buffer object,
    so that its properties or contents can be modified directly.

    \sa isUploaded()
*/
QGLBuffer QGLIndexBuffer::buffer() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer;
}

/*!
    Binds this index buffer to the current GL context.  Returns false if
    binding was not possible, usually because upload() has not been called.

    The buffer must be bound to the same QGLContext current when upload()
    was called, or to another QGLContext that is sharing with it.
    Otherwise, false will be returned from this function.

    \sa release(), upload()
*/
bool QGLIndexBuffer::bind()
{
    Q_D(QGLIndexBuffer);
    return d->buffer.bind();
}

/*!
    Releases this index buffer from the current GL context.

    This function must be called with the same QGLContext current
    as when bind() was called on the index buffer.

    \sa bind()
*/
void QGLIndexBuffer::release()
{
    Q_D(QGLIndexBuffer);
    d->buffer.release();
}

void QGLIndexBufferPrivate::append
    (const QGLIndexBufferPrivate *other, uint offset, int start)
{
    if (elementType == GL_UNSIGNED_SHORT &&
            other->elementType == GL_UNSIGNED_SHORT) {
        // Both buffers are ushort.
        const ushort *data = other->indexesShort.constData() + start;
        int count = other->indexesShort.count() - start;
        indexesShort.reserve(indexesShort.count() + count);
        indexCount += count;
        while (count-- > 0)
            indexesShort.append(ushort(*data++ + offset));
    } else if (elementType == GL_UNSIGNED_SHORT) {
        // Only first buffer is ushort: convert it to int first.
        const ushort *indexes = indexesShort.constData();
        int count = indexesShort.count();
        indexesInt.reserve(count + other->indexesInt.count());
        while (count-- > 0)
            indexesInt.append(*indexes++);
        indexesShort = QArray<ushort>();
        elementType = GL_UNSIGNED_INT;
        const uint *data = other->indexesInt.constData() + start;
        count = other->indexesInt.count() - start;
        indexCount += count;
        while (count-- > 0)
            indexesInt.append(*data++ + offset);
    } else if (other->elementType == GL_UNSIGNED_SHORT) {
        // Only second buffer is ushort.
        const ushort *data = other->indexesShort.constData() + start;
        int count = other->indexesShort.count() - start;
        indexesInt.reserve(indexesInt.count() + count);
        indexCount += count;
        while (count-- > 0)
            indexesInt.append(*data++ + offset);
    } else {
        // Neither buffer is ushort.
        const uint *data = other->indexesInt.constData() + start;
        int count = other->indexesInt.count() - start;
        indexesInt.reserve(indexesInt.count() + count);
        indexCount += count;
        while (count-- > 0)
            indexesInt.append(*data++ + offset);
    }
}

uint QGLIndexBufferPrivate::headIndex(int posn) const
{
    if (indexCount <= posn)
        return uint(-1);
    if (elementType == GL_UNSIGNED_SHORT)
        return indexesShort[posn];
    else
        return indexesInt[posn];
}

uint QGLIndexBufferPrivate::tailIndex(int posn) const
{
    if (indexCount <= posn)
        return uint(-1);
    if (elementType == GL_UNSIGNED_SHORT)
        return indexesShort[indexCount - posn - 1];
    else
        return indexesInt[indexCount - posn - 1];
}

/*!
    Appends the contents of \a buffer to this index buffer and adds
    \a offset to all of the entries in \a buffer.

    This function is typically used to combine multiple geometry meshes
    into a single mesh that can be bound as a single buffer.

    The request is ignored if this index buffer or \a buffer have already
    been uploaded, or \a buffer is this index buffer.

    \sa isUploaded(), setIndexes()
*/
void QGLIndexBuffer::append(const QGLIndexBuffer &buffer, uint offset)
{
    Q_D(QGLIndexBuffer);
    const QGLIndexBufferPrivate *dbuf = buffer.d_ptr;

    // Bail out if the buffers are uploaded or identical.
    if (d->buffer.isCreated() || dbuf->buffer.isCreated())
        return;
    if (d == dbuf)
        return;

    // Append the two index arrays.
    d->append(dbuf, offset, 0);
}

/*!
    Appends the contents of \a buffer to this index buffer and adds
    \a offset to all of the entries in \a buffer.

    The two buffers will be merged at the join point according to
    \a combineMode.  For example, if \a combineMode is QGL::TriangleStrip,
    then the result will be a single triangle strip.  Indexes are
    dropped from the front of \a buffer as necessary to correctly
    merge the buffers.

    This function is typically used to combine multiple geometry meshes
    into a single mesh that can be bound as a single buffer.

    The request is ignored if this index buffer or \a buffer have already
    been uploaded, or \a buffer is this index buffer.

    \sa isUploaded(), setIndexes()
*/
void QGLIndexBuffer::append
    (const QGLIndexBuffer &buffer, uint offset, QGL::DrawingMode combineMode)
{
    Q_D(QGLIndexBuffer);
    const QGLIndexBufferPrivate *dbuf = buffer.d_ptr;

    // Bail out if the buffers are uploaded or identical.
    if (d->buffer.isCreated() || dbuf->buffer.isCreated())
        return;
    if (d == dbuf)
        return;

    // Determine how to combine the buffers.
    switch (int(combineMode)) {
    case QGL::Points:
    case QGL::Lines:
    case QGL::Triangles:
    case QGL::LinesAdjacency:
    case QGL::TrianglesAdjacency:
    case 0x0007:            // GL_QUADS
        // These can be done by just appending the raw data with no changes.
        d->append(dbuf, offset, 0);
        break;

    case QGL::LineLoop:
    case QGL::LineStrip:
    case 0x0009:            // GL_POLYGON
        // Join the last index of the first buffer to the first
        // index of the second buffer to continue the loop or strip.
        if (d->tailIndex(0) == (dbuf->headIndex(0) + offset))
            d->append(dbuf, offset, 1);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleStrip:
        // Join the last two indexes of the first buffer to the first
        // two indexes of the second buffer to continue the strip.
        // It is possible that the first two indexes of the second
        // buffer may be reversed for strip continuation depending
        // upon whether the first strip is odd or even in length.
        if (d->tailIndex(1) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else if (d->tailIndex(1) == (dbuf->headIndex(1) + offset) &&
                 d->tailIndex(0) == (dbuf->headIndex(0) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case 0x0008:            // GL_QUAD_STRIP
        // Join the last two indexes of the first buffer to the first
        // two indexes of the second buffer to continue the strip.
        if (d->tailIndex(1) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleFan:
        // The first index of both buffers should be the same, and the
        // last index of the first buffer should be the same as the second
        // index of the second buffer.
        if (d->headIndex(0) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::LineStripAdjacency:
        // Join the last three indexes of the first buffer to the first
        // three indexes of the second buffer to continue the strip.
        if (d->tailIndex(2) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(1) == (dbuf->headIndex(1) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(2) + offset))
            d->append(dbuf, offset, 3);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleStripAdjacency:
        // Fourth last and second last of first buffer need to be the
        // same as the first and third of the second buffer.
        if (d->tailIndex(3) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(1) == (dbuf->headIndex(2) + offset))
            d->append(dbuf, offset, 4);
        else
            d->append(dbuf, offset, 0);
        break;

    default:
        qWarning("QGLIndexBuffer::append: unknown drawing mode 0x%04x",
                 int(combineMode));
        break;
    }
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume all of the elements of \a indexes,
    which are used to index into the enabled arrays.

    If \a indexes has not been uploaded to the GL server as an index
    buffer, then this function will draw using a client-side array.

    \sa update(), QGLIndexBuffer::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexBuffer& indexes)
{
    QGLIndexBufferPrivate *d = const_cast<QGLIndexBufferPrivate *>(indexes.d_func());
    update();
    GLuint id = d->buffer.bufferId();
    if (id != d_ptr->boundIndexBuffer) {
        if (id)
            d->buffer.bind();
        else
            QGLBuffer::release(QGLBuffer::IndexBuffer);
        d_ptr->boundIndexBuffer = id;
    }
    if (id) {
        glDrawElements(GLenum(mode), d->indexCount, d->elementType, 0);
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT,
                       d->indexesShort.constData());
    } else {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_INT,
                       d->indexesInt.constData());
    }
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume \a count elements of \a indexes,
    starting at \a offset, which are used to index into the enabled arrays.

    If \a indexes has not been uploaded to the GL server as an index
    buffer, then this function will draw using a client-side array.

    \sa update(), QGLIndexBuffer::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexBuffer& indexes, int offset, int count)
{
    QGLIndexBufferPrivate *d = const_cast<QGLIndexBufferPrivate *>(indexes.d_func());
    update();
    GLuint id = d->buffer.bufferId();
    if (id != d_ptr->boundIndexBuffer) {
        if (id)
            d->buffer.bind();
        else
            QGLBuffer::release(QGLBuffer::IndexBuffer);
        d_ptr->boundIndexBuffer = id;
    }
    if (id) {
        if (d->elementType == GL_UNSIGNED_SHORT) {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void *>(offset * sizeof(ushort)));
        } else {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       reinterpret_cast<const void *>(offset * sizeof(int)));
        }
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       d->indexesShort.constData() + offset);
    } else {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       d->indexesInt.constData() + offset);
    }
}

QT_END_NAMESPACE
