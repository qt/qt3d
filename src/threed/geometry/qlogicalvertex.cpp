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

#include "qlogicalvertex.h"
#include "qvector_utils_p.h"

#include <QtCore/qdebug.h>

/*!
    \class QLogicalVertex
    \brief The QLogicalVertex class references QGeometryData at a single vertex.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    QLogicalVertex instances are a convenience class for use with
    QGeometryData.  A QLogicalVertex simply references through to the data
    in a QGeometryData for a particular vertex, providing accessors to fetch
    position, texture coordinates, and other values.

    Create a QLogicalVertex referring to a particular QGeometryData instance:
    \code
    QGeometryData data;
    data.appendVertex(QVector3D(1, 2, 3));

    // construct a QLogicalVertex referring to the first vertex in data
    // the QGeometryData is implicitly shared with lv
    QLogicalVertex lv(data, 0);
    //  lv.vertex() == QVector3D(1, 2, 3)
    \endcode
    This is inexpensive and no new storage is allocated for the actual data,
    just the reference and index.

    With logical vertices instances referencing large QGeometryData instances,
    avoid modifying the instance:
    \code
    // careful - assigning to a QLogicalVertex which refers to an external
    // QGeometryData will result in a possibly expensive copy-on-write
    lv.setVertex(3, 2, 1);
    \endcode

    Create a QLogicalVertex with its own QGeometryData internally:
    \code
    QLogicalVertex lv;
    // no copy on write here - internal QGeometryData is not shared
    lv.setVertex(1, 2, 3);
    \endcode

    Assign an instance of QLogicalVertex:
    \code
    QLogicalVertex lv2;
    lv2 = data.logicalVertexAt(0);
    \endcode
    Although lv2 gets its own internal QGeometryData which is then immediately
    thrown away by the assignment, because of lazy initialization in
    QGeometryData the cost is negligible.

    Use the fields() and hasField() functions to determine if a particular
    field is present in the vertex.  Accessing non-existent data will cause
    an assert in debug mode (from the underlying QArray), and give
    undefined behaviour in release mode.

    \sa QGeometryData, QGLBuilder
*/

/*!
    \fn QLogicalVertex::QLogicalVertex()
    Constructs a new invalid QLogicalVertex which has no data.
*/

/*!
    \fn QLogicalVertex::QLogicalVertex(QGeometryData data, int index)
    Constructs a new QLogicalVertex referencing the \a data at \a index.
    Note that if this QLogicalVertex is modified, by calling vertex() or
    setNormal() for example, then a copy-on-write for \a data will be
    triggered.
*/

/*!
    \fn QLogicalVertex::QLogicalVertex(const QVector3D &a)
    Constructs a new QLogicalVertex with a vertex set to \a a.
*/

/*!
    \fn QLogicalVertex::QLogicalVertex(const QVector3D &a, const QVector3D &n);
    Constructs a new QLogicalVertex with a vertex set to \a a, and normal set to \a n.
*/

/*!
    \fn QLogicalVertex::QLogicalVertex(const QVector3D &a, const QVector3D &n, const QVector2D &t)
    Constructs a new QLogicalVertex with its vertex value set to \a a, normal set
    to \a n, and texture set to \a t.  By default \a n is the null QVector3D,
    and \a t is the InvalidTexCoord.  If \a n is null then hasType(QLogicalVertex::Normal)
    will return false.  Likewise if \a t is the InvalidTexCoord then
    hasType(QLogicalVertex::Texture) will return false.
*/

/*!
    \fn QLogicalVertex::QLogicalVertex(const QVector3D &a, QColor4ub color)
    Constructs a new QLogicalVertex with its vertex value set to \a a,
    color value set to \a color.
*/

/*!
    \fn QLogicalVertex::~QLogicalVertex()
    Destroys this QLogicalVertex reclaiming any resources.
*/

/*!
    \fn const QVector3D &QLogicalVertex::vertex() const
    Returns a const reference to the vertex value for this vertex.
*/

/*!
    \fn void QLogicalVertex::setVertex(const QVector3D &v)
    Sets the vertex value for this vertex to \a v.
*/

/*!
    \fn QVector3D &QLogicalVertex::vertex()
    Returns a modifiable reference to the vertex value.
*/

/*!
    \fn QLogicalVertex::operator QVector3D ()
    Returns a copy of the vertex value, by casting as a QVector3D.  This
    allows passing of a QLogicalVertex to functions that expect a QVector3D.
*/

/*!
    \fn QVariant QLogicalVertex::attribute(QGL::VertexAttribute field) const
    Returns the attribute value for \a field.  The \a field defaults
    to QGL::CustomVertex0.
*/

/*!
    \fn void QLogicalVertex::setAttribute(float value, QGL::VertexAttribute field)
    Sets the float attribute \a value at \a field.  The \a field
    defaults to QGL::CustomVertex0.
*/

/*!
    \fn void QLogicalVertex::setAttribute(const QVector2D &v, QGL::VertexAttribute field)
    Sets the QVector2D attribute \a v at \a field.  The \a field
    defaults to QGL::CustomVertex0.
*/

/*!
    \fn void QLogicalVertex::setAttribute(const QVector3D &v, QGL::VertexAttribute field)
    Sets the QVector3D attribute \a v at \a field.  The \a field
    defaults to QGL::CustomVertex0.
*/

/*!
    \fn float &QLogicalVertex::floatAttribute(QGL::VertexAttribute field)
    Returns a modifiable reference to the attribute at \a field, which
    must be a float.  The \a field defaults to QGL::CustomVertex0.
*/

/*!
    \fn QVector2D &QLogicalVertex::vector2DAttribute(QGL::VertexAttribute field)
    Returns a modifiable reference to the attribute at \a field, which
    must be a QVector2D.  The \a field defaults to QGL::CustomVertex0.
*/

/*!
    \fn QVector3D &QLogicalVertex::vector3DAttribute(QGL::VertexAttribute field = QGL::CustomVertex0);
    Returns a modifiable reference to the attribute at \a field, which
    must be a QVector3D.  The \a field defaults to QGL::CustomVertex0.
*/

/*!
    \fn float QLogicalVertex::floatAttribute(QGL::VertexAttribute field) const
    Returns the attribute at \a field.  The \a field defaults to QGL::CustomVertex0.
    The attribute must be a float value.
*/

/*!
    \fn QVector2D QLogicalVertex::vector2DAttribute(QGL::VertexAttribute field) const
    Returns the attribute at \a field.  The \a field defaults to QGL::CustomVertex0.
    The attribute must be a QVector2D value.
*/

/*!
    \fn QVector3D QLogicalVertex::vector3DAttribute(QGL::VertexAttribute field) const
    Returns the attribute at \a field.  The \a field defaults to QGL::CustomVertex0.
    The attribute must be a QVector3D value.
*/

/*!
    \fn QCustomDataArray::ElementType QLogicalVertex::attributeType(QGL::VertexAttribute field)
    Returns the element type for the attribute \a field.
*/

/*!
    \fn const QVector3D &QLogicalVertex::normal() const
    Returns a const reference to the normal value for this vertex.
*/

/*!
    \fn void QLogicalVertex::setNormal(const QVector3D &n)
    Sets the normal value for this vertex to \a n.
*/

/*!
    \fn QVector3D &QLogicalVertex::normal()
    Returns a modifiable reference to the normal value for this vertex.
*/

/*!
    \fn QVector2D QLogicalVertex::texCoord(QGL::VertexAttribute field) const
    Returns a copy of the texture coordinate value at \a field for this vertex.
    The \a field defaults to QGL::TextureCoord0.
*/

/*!
    \fn void QLogicalVertex::setTexCoord(const QVector2D &t, QGL::VertexAttribute field)
    Sets the texture coordinate at \a field for this vertex to \a t.
    The \a field defaults to QGL::TextureCoord0.
*/

/*!
    \fn QVector2D &QLogicalVertex::texCoordRef(QGL::VertexAttribute field)
    Returns a modifiable reference to the texture coordinate for this vertex.
    The \a field defaults to QGL::TextureCoord0.
*/

/*!
    \fn QColor4ub QLogicalVertex::color() const
    Returns a const reference to the color value for this vertex.
*/

/*!
    \fn void QLogicalVertex::setColor(const QColor4ub &c)
    Sets the color value for this vertex to \a c.
*/

/*!
    \fn QColor4ub &QLogicalVertex::colorRef()
    Returns a modifiable reference to the color value for this vertex.
*/

/*!
    \fn bool QLogicalVertex::hasField(QGL::VertexAttribute type) const
    Returns true if this vertex has data field \a type, and false otherwise.

    In general check to see if a logical vertex has a particular field
    type before attempting to access it.  In debug mode accessing a
    non-existent field will cause an assert; but in release mode the
    behaviour is undefined.
*/

/*!
    \fn quint32 QLogicalVertex::fields() const
    Returns a bit-mask of the fields in this logical vertex.  Test the
    fields like this:
    \code
    if (vertex.fields() & QGL::fieldMask(QGL::TextureCoord0))
        tex = vertex.texCoord();
    \endcode

    \sa QGeometryData::fields()
*/

/*!
    \fn int QLogicalVertex::index() const
    Returns the index at which this logical vertex's data is located in
    its associated QGeometryData; or -1 if this vertex is null.
*/

/*!
    \fn QGeometryData QLogicalVertex::data() const
    Returns a copy of the QGeometryData underlying this vertex.  Note that
    the copy is not expensive in terms of performance due to implicit sharing
    unless the copy is modified (causing a copy-on-write).

    \sa QLogicalVertex::index()
*/

/*!
   \fn QGeometryData data() const
   Returns a copy of the QGeometryData associated with this vertex.  Note
   that as long as the copy is not modified, this method is not expensive.
*/

/*!
    \fn bool QLogicalVertex::isNull() const
    Returns true if this vertex is null.

    \sa QLogicalVertex()
*/

/*!
    Returns true if \a rhs has exactly the same fields as this logical
    vertex, and each of those are equal to the corresponding field of the \a rhs.

    If either are null, then false is returned.
*/
bool QLogicalVertex::operator==(const QLogicalVertex &rhs) const
{
    if (isNull() || rhs.isNull())
        return false;
    if (this == &rhs)
        return true;
    if (m_data.fields() != rhs.fields())
        return false;
    const quint32 mask = 0x01;
    quint32 fields = m_data.fields();
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                {
                    if (!qFskCompare(vertex(), rhs.vertex()))
                        return false;
                }
                else if (attr == QGL::Normal)
                {
                    if (!qFskCompare(normal(), rhs.normal()))
                        return false;
                }
                else
                {
                    if (color() != rhs.color())
                        return false;
                }
            }
            else if (attr < QGL::CustomVertex0)
            {
                if (!qFskCompare(texCoord(attr), rhs.texCoord(attr)))
                    return false;
            }
            else
            {
                if (attribute(attr) != rhs.attribute(attr))
                    return false;
            }
        }
    }
    return true;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLogicalVertex &lv)
{
    dbg.nospace();
    dbg << "QLogicalVertex(";
    if (lv.isNull())
    {
        dbg << " NULL";
    }
    else
    {
        if (lv.hasField(QGL::Position))
            dbg << "V:" << QVector3D(lv.vertex());
        else
            dbg << " (No Vertex)";
        if (lv.hasField(QGL::Normal))
            dbg << "N:" << QVector3D(lv.normal());
        else
            dbg << " (No Normal)";
        if (lv.hasField(QGL::TextureCoord0))
            dbg << "T:" << QVector2D(lv.texCoord());
        else
            dbg << " (No Texture)";
        if (lv.hasField(QGL::Color))
            dbg << "C:" << QColor4ub(lv.color());
        else
            dbg << " (No Color)";
    }
    dbg << " )";
    return dbg.space();
}
#endif
