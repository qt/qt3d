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

#include "qglattributevalue.h"
#include "qopenglfunctions.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLAttributeValue
    \brief The QGLAttributeValue class encapsulates information about an OpenGL attribute value.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::arrays

    OpenGL has many functions that take a pointer to vertex attribute
    values: \c{glVertexPointer()}, \c{glNormalPointer()},
    \c{glVertexAttribPointer()}, etc.  These functions typically
    take four arguments: tuple size (1, 2, 3, or 4), component type
    (e.g. GL_FLOAT), stride, and data pointer (\c{glNormalPointer()}
    does not use tuple size, assuming that it is 3).  When used with
    vertex buffers, the data pointer may be an offset into the vertex
    buffer instead.

    QGLAttributeValue encapsulates these four values so that they can
    be easily manipulated as a set during OpenGL painting operations.
    Constructors are provided for converting QArray and
    QCustomDataArray objects into an attribute value.

    Because the data() value is a raw pointer to arbitrary memory,
    care should be taken that the memory remains valid until the
    QGLAttributeValue is no longer required.

    \sa QGLAttributeDescription, QArray, QCustomDataArray
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue()

    Constructs a null attribute value with default parameters of
    tupleSize(), and stride() set to zero, type() set to GL_FLOAT,
    and data() set to null.

    \sa isNull()
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<float>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 1, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector2D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 2, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector3D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 3, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QVector4D>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 4, type() to GL_FLOAT, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QArray<QColor4ub>& array)

    Constructs an attribute value that refers to the contents of \a array,
    setting tupleSize() to 4, type() to GL_UNSIGNED_BYTE, and stride() to zero.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/

/*!
    Constructs an attribute value that refers to the contents of \a array.
    The tupleSize() and type() of the attribute value will be set according
    to the QCustomDataArray::elementType() of \a array.

    The \a array must not be destroyed or modified until the attribute
    value is no longer required.
*/
QGLAttributeValue::QGLAttributeValue(const QCustomDataArray& array)
    : m_tupleSize(0), m_type(GL_FLOAT), m_stride(0)
    , m_data(array.data()), m_count(array.count())
{
    switch (array.elementType()) {
    case QCustomDataArray::Float:
        m_tupleSize = 1;
        break;
    case QCustomDataArray::Vector2D:
        m_tupleSize = 2;
        break;
    case QCustomDataArray::Vector3D:
        m_tupleSize = 3;
        break;
    case QCustomDataArray::Vector4D:
        m_tupleSize = 4;
        break;
    case QCustomDataArray::Color:
        m_tupleSize = 4;
        m_type = GL_UNSIGNED_BYTE;
        break;
    }
}

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int tupleSize, GLenum type, int stride, const void *data, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a data, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(int tupleSize, GLenum type, int stride, int offset, int count)

    Constructs an attribute value with the fields \a tupleSize, \a type,
    \a stride, \a offset, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QGLAttributeDescription& description, const void *data, int count)

    Constructs an attribute value with the supplied \a description,
    \a data, and \a count.
*/

/*!
    \fn QGLAttributeValue::QGLAttributeValue(const QGLAttributeDescription& description, int offset, int count)

    Constructs an attribute value with the supplied \a description,
    \a offset, and \a count.
*/

/*!
    \fn bool QGLAttributeValue::isNull() const

    Returns true if tupleSize() is zero, which indicates an unset
    attribute value; false otherwise.

    Note: it is possible for data() to be null, but isNull() returns true.
    This can happen when data() is actually a zero offset into a
    vertex buffer.
*/

/*!
    \fn QGLAttributeDescription QGLAttributeValue::description(QGL::VertexAttribute attribute) const

    Returns the description of this value, tagged with \a attribute.

    \sa type()
*/

/*!
    \fn GLenum QGLAttributeValue::type() const

    Returns the component type for this attribute value.  The default
    value is GL_FLOAT.

    \sa sizeOfType(), description()
*/

/*!
    Returns the size in bytes of type().

    \sa type(), tupleSize()
*/
int QGLAttributeValue::sizeOfType() const
{
    switch (m_type) {
    case GL_BYTE:           return int(sizeof(GLbyte));
    case GL_UNSIGNED_BYTE:  return int(sizeof(GLubyte));
    case GL_SHORT:          return int(sizeof(GLshort));
    case GL_UNSIGNED_SHORT: return int(sizeof(GLushort));
    case GL_INT:            return int(sizeof(GLint));
    case GL_UNSIGNED_INT:   return int(sizeof(GLuint));
    case GL_FLOAT:          return int(sizeof(GLfloat));
#if defined(GL_DOUBLE) && !defined(QT_OPENGL_ES)
    case GL_DOUBLE:         return int(sizeof(GLdouble));
#endif
    default:                return 0;
    }
}

/*!
    \fn int QGLAttributeValue::tupleSize() const

    Returns the tuple size of this attribute in components.  For example,
    a return value of 3 indicates a vector of 3-dimensional values.
    If tupleSize() is zero, then this attribute value is null.

    \sa isNull(), sizeOfType()
*/

/*!
    \fn int QGLAttributeValue::stride() const

    Returns the stride in bytes from one vertex element to the
    next for this attribute value.  The default value of 0 indicates
    that the elements are tightly packed within the data() array.
*/

/*!
    \fn const void *QGLAttributeValue::data() const

    Returns the data pointer for the elements in this attribute value.
*/

/*!
    \fn int QGLAttributeValue::count() const

    Returns the count of vertex elements in this attribute value;
    zero if the count is unknown.
*/

QT_END_NAMESPACE
