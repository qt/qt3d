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

#include "qgeometrydata.h"
#include "qlogicalvertex.h"
#include "qglpainter.h"

#include <QtOpenGL/qgl.h>
#include <QtCore/qdebug.h>

/*!
    \class QGeometryData
    \brief The QGeometryData class encapsulates sets of geometry data.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QGeometryData class encloses a number of data arrays
    that model most typical vertex data needs.  The class provides a
    store for all of the data types in the QGL::VertexAttribute enumeration.

    \table
        \header
            \o QGL::VertexAttribute
            \o QGeometryData functions
        \row
            \o QGL::Position
            \o appendVertex(), vertex(), vertices()
        \row
            \o QGL::Normal
            \o appendNormal(), normal(), normals()
        \row
            \o QGL::Color
            \o appendColor(), colorRef(), colors()
        \row
            \o QGL::TextureCoord0 - QGL::TextureCoord3
            \o appendTexCoord(), texCoordRef(), texCoords()
        \row
            \o QGL::CustomVertex0 - QGL::CustomVertex1, QGL::UserVertex
            \o appendAttribute(), vector3DAttribute(), attributes()
     \endtable

     Additionally the class provides the following features:
     \list
        \o appendVertex() for adding a QLogicalVertex()
        \o logicalVertexAt() for return the data at an index as a QLogicalVertex()
        \o hasField() to find if a particular data type is present
        \o normalizeNormals() to reduce all normal vectors to unit length
        \o boundingBox() to find the bounds of the geometry
     \endlist

     It is up to the user of a QGeometryData instance to ensure that the
     data has an equal number of items in each field.  For example, if five
     vertices are added and only two normals are added, the logical vertex at
     position 3 will be corrupt, since it does not have a normal.

     While data is being accumulated the counts of different fields will vary,
     since it may be convenient to add several vertices, then several normals,
     colors or attributes at a time.  However when a logical vertex is
     constructed or when the data is sent to the GPU, counts of all fields
     must be equal.

     QGeometryData uses explicit sharing with lazy creation of internal
     data so that code like:
     \code
     QGeometryData myData;
     if (processed)
        myData = processedData();
     \endcode
     is very inexpensive, since the first declaration and initialization
     does not cause internal data to be created (only to be overwritten by the
     assignment operation).

     Since QGeometryData is explicitly shared, variables of type
     QGeometryData behave like references, and the underlying data is modified
     by calling a non-const function on any variable which shares that data.

     To force an explicit copy call the detach() function.
*/

/*!
    \typedef QGL::IndexArray

    This is a convenience for either QArray<ushort> (OpenGL/ES) or
    QArray<int> (desktop OpenGL).
*/

class QGeometryDataPrivate
{
public:
    QGeometryDataPrivate();
    ~QGeometryDataPrivate();
    QGeometryDataPrivate *clone() const;

    QBasicAtomicInt ref;

    QVector3DArray vertices;
    QVector3DArray normals;
    QArray<QColor4ub> colors;
    QList<QCustomDataArray> attributes;
    QList<QVector2DArray> textures;
    QGL::IndexArray indices;
    QGLVertexBundle vertexBundle;
    QGLIndexBuffer indexBuffer;
    bool uploadsViable;
    bool modified;
    QBox3D bb;
    static const int ATTR_CNT = 32;
    quint32 fields;
    qint8 key[ATTR_CNT];
    quint8 size[ATTR_CNT];
    int count;
    int reserved;
    bool boxValid;
    QGeometryData::BufferStrategy bufferStrategy;
};

QGeometryDataPrivate::QGeometryDataPrivate()
    : uploadsViable(true)
    , modified(false)
    , fields(0)
    , count(0)
    , reserved(-1)
    , boxValid(true)
    , bufferStrategy(QGeometryData::BufferIfPossible | QGeometryData::KeepClientData)
{
    ref = 0;
    qMemSet(key, -1, ATTR_CNT);
    qMemSet(size, 0, ATTR_CNT);
}

QGeometryDataPrivate::~QGeometryDataPrivate()
{
}

QGeometryDataPrivate *QGeometryDataPrivate::clone() const
{
    QGeometryDataPrivate *temp = new QGeometryDataPrivate;
    temp->vertices = vertices;
    temp->normals = normals;
    temp->colors = colors;
    temp->attributes = attributes;
    temp->textures = textures;
    temp->indices = indices;
    temp->vertexBundle = vertexBundle;
    temp->indexBuffer = indexBuffer;
    temp->uploadsViable = uploadsViable;
    temp->modified = modified;
    temp->bb = bb;
    temp->fields = fields;
    qMemCopy(temp->key, key, ATTR_CNT);
    qMemCopy(temp->size, size, ATTR_CNT);
    temp->count = count;
    temp->reserved = reserved;
    temp->boxValid = boxValid;
    temp->bufferStrategy = bufferStrategy;
    return temp;
}

/*!
    \fn quint32 QGL::fieldMask(QGL::VertexAttribute attribute)
    \relates QGeometryData
    Returns an unsigned integer mask from the \a attribute.

    \sa QGeometryData::fields()
*/

/*!
    \enum QGeometryData::BufferStrategyFlags

    This enum serves to describe how management of the data is handled
    with respect to vertex buffer objects.  The strategies are essentially a
    combination of whether the client data is kept around after it has been
    successfully uploaded to the GPU; and whether an upload is attempted at
    all.

    If the data set is very small it may be pointless to use up a VBO, hence
    in this case KeepClientData may be used resulting in no attempt to upload
    the data and client side arrays used instead.

    \value InvalidStrategy No valid strategy has been specified.
    \value KeepClientData Keep the client data, even after successful upload to the GPU.
    \value BufferIfPossible Try to upload the data to the GPU.
*/

/*!
    Construct an empty QGeometryData
*/
QGeometryData::QGeometryData()
    : d(0)
{
}

/*!
    Construct QGeometryData as a copy of \a other
*/
QGeometryData::QGeometryData(const QGeometryData &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    Construct an empty QGeometryData with the \a fields enabled.
*/
QGeometryData::QGeometryData(quint32 fields)
    : d(new QGeometryDataPrivate)
{
    d->ref.ref();
    const quint32 mask = 0x01;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (!(mask & fields)) continue;
        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
        enableField(attr);
    }
}

/*!
    Destroys this QGeometryData recovering any resources.
*/
QGeometryData::~QGeometryData()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns this QGeometryData to be a copy of \a other.
*/
QGeometryData &QGeometryData::operator=(const QGeometryData &other)
{
    if (d != other.d)
    {
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
        if (d)
            d->ref.ref();
    }
    return *this;
}

/*!
    Appends the geometry in \a data to this.  If this is empty, then all
    fields of \a data are appended; otherwise (when this has existing fields)
    only those fields that exist in both are appended.

    This does not change the indices - to reference the new geometry add
    indices via the appendIndices() functions.
*/
void QGeometryData::appendGeometry(const QGeometryData &data)
{
    if (data.d && data.count())
    {
        detach();
        d->modified = true;
        d->boxValid = false;
        int cnt = data.d->count;
        const quint32 mask = 0x01;
        quint32 fields = d->fields | data.fields();
        d->fields = fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                enableField(attr);  // might not be enabled if we had NO fields
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                        d->vertices.append(data.d->vertices);
                    else if (attr == QGL::Normal)
                        d->normals.append(data.d->normals);
                    else  // colors
                        d->colors.append(data.d->colors);
                }
                else if (attr < QGL::CustomVertex0)
                {
                    d->textures[d->key[attr]].append(data.texCoords(attr));
                }
                else
                {
                    d->attributes[d->key[attr]].append(data.attributes(attr));
                }
            }
        }
        d->count += cnt;
    }
}

/*!
    Appends all the data fields in QLogicalVertex \a v to this
    QGeometryData object.
*/
int QGeometryData::appendVertex(const QLogicalVertex &v)
{
    create();
    d->modified = true;
    if (d->boxValid)
        d->bb.unite(v.vertex());
    quint32 fields = v.fields();
    const quint32 mask = 0x01;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                    appendVertex(v.vertex());
                else if (attr == QGL::Normal)
                    appendNormal(v.normal());
                else
                    appendColor(v.color());
            }
            else if (attr < QGL::CustomVertex0)
            {
                appendTexCoord(v.texCoord(attr), attr);
            }
            else
            {
                appendAttribute(v.attribute(attr), attr);
            }
        }
    }
    return d->count - 1;
}

/*!
    Returns a QLogicalVertex that references the \a{i}'th logical vertex
    of this geometry.
*/
QLogicalVertex QGeometryData::logicalVertexAt(int i) const
{
    return QLogicalVertex(*this, i);
}

/*!
    Normalize all the normal vectors in this geometry to unit length.
*/
void QGeometryData::normalizeNormals()
{
    check();
    if (d)  // nothng to do if its null
    {
        create();
        d->modified = true;
        if (hasField(QGL::Normal))
        {
            for (int i = 0; i < d->normals.count(); ++i)
                d->normals[i].normalize();
        }
    }
}

/*!
    Calculate and return a bounding box for the vertex data in this geometry.
*/
QBox3D QGeometryData::boundingBox() const
{
    QBox3D box;
    if (d)
    {
        if (d->boxValid)
        {
            box = d->bb;
        }
        else
        {
            for (int i = 0; i < d->count; ++i)
                box.unite(d->vertices.at(i));
            d->bb = box;
        }
    }
    return box;
}

/*!
    Returns the coordinates of the center of the geometry.

    The center is calculated as the centroid or geometric barycenter
    of the vertices (the average of the vertices).  For a convex hull this
    is guaranteed to be inside the figure.
*/
QVector3D QGeometryData::center() const
{
    QVector3D center;
    for (int i = 0; i < d->vertices.count(); ++i)
        center += d->vertices.at(i);
    return center / (float)d->vertices.count();
}

/*!
    Returns a copy of this geometry data with elements in reverse order.
*/
QGeometryData QGeometryData::reversed() const
{
    QGeometryData r;
    for (int i = count() - 1; i >= 0; --i)
        r.appendVertex(logicalVertexAt(i));
    return r;
}

/*!
    Returns a copy of this geometry data with QGL::Position data translated by
    the vector \a t.  The other fields are unchanged.
*/
QGeometryData QGeometryData::translated(const QVector3D &t) const
{
    QGeometryData r(*this);
    r.detach();
    for (int i = 0; i < count(); ++i)
    {
        r.vertex(i) = r.vertexAt(i) + t;
    }
    return r;
}

/*!
    Modifies this geometry data by generating texture data based on QGL::Position
    values.  If \a orientation is Qt::Horizontal (the default) then x-coordinate
    values are generated, and y-coordinate values are set to 0.0; otherwise
    y-coordinate values are generated and x-coordinate values are set to 0.0.
    The values are appended to the texture coordinate \a field.

    The method of calculation is based on the assumption that the vertex data
    is a list of extents which span across the texture space horizontally, from
    x = 0.0 to x = 1.0, in the case of Qt::Horizontal; or vertically in the
    case of Qt::Vertical.  The texture space of 1.0 is divided up proportionately
    by the length of each extent.

    \image texture-coords-gen.png

    In this diagram the large blue numbers are the lengths of each extent, and
    the texture coordinates generated are shown as \c{t(7/16, 1)} and so on.

    Thus the texture coordinate t0 for vertex v0, is 0.0; t1 for vertex v1 is
    \c{(v1 - v0).length() / totalLength} and so on.

    The code to produce the texture coordinates for the quads in the image is:
    \code
    QGeometryData top;

    // add data to the primitive
    top.appendVertex(QVector3D(0.0, 0.0, 0.0));
    top.appendVertex(QVector3D(6.0, 3.6, 0.0));    // (v1 - v0).length() = 7.0
    top.appendVertex(QVector3D(10.0, 0.6, 0.0));   // (v2 - v1).length() = 5.0
    top.appendVertex(QVector3D(13.0, 3.24, 0.0));  // (v3 - v2).length() = 4.0

    // generate x (Qt::Horizontal) texture coordinates over the primitive
    top.generateTextureCoordinates();              // spread over 7 + 5 + 4 = 16

    // make a copy translated down, the copy has y texture coordinates all 0
    QGeometryData bottom = top.translated(QVector3D(0, 0, -1));

    // now modify the top so its y texture coordinates are all 1
    for (int i = 0; i < top.count(); ++i)
        top.texCoordRef(QGL::TextureCoord0).setY(1.0);

    displayList->addQuadsZipped(top, bottom);
    \endcode
*/
void QGeometryData::generateTextureCoordinates(Qt::Orientation orientation, QGL::VertexAttribute field)
{
    QArray<qreal> extents;
    extents.append(0.0);
    qreal totalExtents = 0.0;
    QArray<QVector3D> v = vertices();
    for (int i = 0; i < v.count() - 1; ++i)
    {
        int n = (i + 1) % v.count();
        QVector3D e = v[n] - v[i];
        qreal extent = e.length();
        totalExtents += extent;
        extents.append(totalExtents);
    }
    if (hasField(field))
        clear(field);
    if (orientation == Qt::Horizontal)
    {
        for (int i = 0; i < v.count(); ++i)
            appendTexCoord(QVector2D(extents[i] / totalExtents, 0.0), field);
    }
    else
    {
        for (int i = 0; i < v.count(); ++i)
            appendTexCoord(QVector2D(0.0, extents[i] / totalExtents), field);
    }
}

/*!
    Returns a QGeometryData instance containing alternating vertices from
    this geometry and \a other.  The resulting geometry contains N vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both geometries.
*/
QGeometryData QGeometryData::interleavedWith(const QGeometryData &other) const
{
    QGeometryData res;
    check();
    other.check();
    if (d && other.d)
    {
        int cnt = qMax(d->count, other.d->count);
        const quint32 mask = 0x01;
        quint32 fields = d->fields & other.d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                res.enableField(attr);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        res.d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        res.d->normals = tmp;
                    }
                    else  // colors
                    {
                        QArray<QColor4ub> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->colors.at(i));
                            tmp.append(other.d->colors.at(i));
                        }
                        res.d->colors = tmp;
                    }
                }
                else if (attr < QGL::CustomVertex0)
                {
                    QArray<QVector2D> tmp;
                    const QArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(txa.at(i));
                        tmp.append(txb.at(i));
                    }
                    res.d->textures[d->key[attr]] = tmp;
                }
                else
                {
                    QCustomDataArray tmp;
                    const QCustomDataArray ata = d->attributes.at(d->key[attr]);
                    const QCustomDataArray atb = other.d->attributes.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(ata.at(i));
                        tmp.append(atb.at(i));
                    }
                    res.d->attributes[d->key[attr]] = tmp;
                }
            }
        }
        res.d->count = cnt * 2;
    }
    return res;
}

/*!
    Sets this QGeometryData to contain alternating vertices from
    this geometry and \a other.  The resulting geometry contains \c{N * 2} vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both geometries.
*/
void QGeometryData::interleaveWith(const QGeometryData &other)
{
    check();
    other.check();
    if (d && other.d)
    {
        create();
        d->modified = true;
        d->boxValid = false;
        int cnt = qMin(d->count, other.d->count);
        const quint32 mask = 0x01;
        quint32 fields = d->fields & other.d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->vertices.at(i));
                            tmp.append(other.d->vertices.at(i));
                        }
                        d->vertices = tmp;
                    }
                    else if (attr == QGL::Normal)
                    {
                        QArray<QVector3D> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->normals.at(i));
                            tmp.append(other.d->normals.at(i));
                        }
                        d->normals = tmp;
                    }
                    else  // colors
                    {
                        QArray<QColor4ub> tmp;
                        for (int i = 0; i < cnt; ++i)
                        {
                            tmp.append(d->colors.at(i));
                            tmp.append(other.d->colors.at(i));
                        }
                        d->colors = tmp;
                    }
                }
                else if (attr < QGL::CustomVertex0)
                {
                    QArray<QVector2D> tmp;
                    const QArray<QVector2D> txa = d->textures.at(d->key[attr]);
                    const QArray<QVector2D> txb = other.d->textures.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(txa.at(i));
                        tmp.append(txb.at(i));
                    }
                    d->textures[d->key[attr]] = tmp;
                }
                else
                {
                    QCustomDataArray tmp;
                    const QCustomDataArray ata = d->attributes.at(d->key[attr]);
                    const QCustomDataArray atb = other.d->attributes.at(other.d->key[attr]);
                    for (int i = 0; i < cnt; ++i)
                    {
                        tmp.append(ata.at(i));
                        tmp.append(atb.at(i));
                    }
                    d->attributes[d->key[attr]] = tmp;
                }
            }
        }
        d->count = cnt * 2;
    }
}

/*!
    Clear all data structures.  The actual fields are retained, but they
    have no contents.
    \code
    QGeometryData data;
    data.appendVertex(a);
    data.appendTexCoord(t);

    // prints "1"
    qDebug() << data.count();

    // x == a
    QVector3D x = data.vertexAt(0);

    quint32 flds = QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::TextureCoord0);
    qDebug() << (flds == data.fields());  // prints "true"

    data.clear();
    qDebug() << data.count();             // prints "0"
    QVector3D x = data.vertexAt(0);       // asserts - no data in vertices
    qDebug() << (flds == data.fields());  // still prints "true"
    \endcode

    To clear a specific field and its data use \c{data.clear(field)} below.

    To clear all fields and data, simply set this to an empty geometry:
    \code
    data = QGeometryData();
    \endcode
 */
void QGeometryData::clear()
{
    if (d)
    {
        create();
        d->modified = true;
        d->bb = QBox3D();
        d->boxValid = true;
        const quint32 mask = 0x01;
        quint32 fields = d->fields;
        for (int field = 0; fields; ++field, fields >>= 1)
        {
            if (mask & fields)
            {
                QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                if (attr < QGL::TextureCoord0)
                {
                    if (attr == QGL::Position)
                        d->vertices.clear();
                    else if (attr == QGL::Normal)
                        d->normals.clear();
                    else
                        d->colors.clear();
                }
                else if (attr < QGL::CustomVertex0)
                {
                    d->textures[d->key[field]].clear();
                }
                else
                {
                    d->attributes[d->key[field]].clear();
                }
            }
        }
        d->count = 0;
    }
}

/*!
    Clears the data from \a field, and removes the field.  After this call
    hasField() will return false for this field.
*/
void QGeometryData::clear(QGL::VertexAttribute field)
{
    if (d && (QGL::fieldMask(field) & d->fields))
    {
        create();
        d->modified = true;
        if (field == QGL::Position)
        {
            d->bb = QBox3D();
            d->boxValid = true;
        }
        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
        if (attr < QGL::TextureCoord0)
        {
            if (attr == QGL::Position)
                d->vertices.clear();
            else if (attr == QGL::Normal)
                d->normals.clear();
            else
                d->colors.clear();
        }
        else if (attr < QGL::CustomVertex0)
        {
            d->textures[d->key[field]].clear();
        }
        else
        {
            d->attributes[d->key[field]].clear();
        }
        d->key[field] = -1;
        d->fields = d->fields & ~QGL::fieldMask(field);
    }
}

/*!
    Sets the geometry data to handle an \a amount of data.  This is generally
    not required unless its anticipated that a large amount of data will be
    appended and realloc overhead is desired to be avoided.  If \a amount is
    less than the amount already reserved, or if this object has
    more than the \a amount of data items, then this function exits without
    doing anything.  This function will never delete data.
*/
void QGeometryData::reserve(int amount)
{
    if (d && (d->reserved > amount || d->reserved < d->count))
        return;
    create();
    d->reserved = amount;
    const quint32 mask = 0x01;
    quint32 fields = d->fields;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                    d->vertices.reserve(amount);
                else if (attr == QGL::Normal)
                    d->normals.reserve(amount);
                else
                    d->colors.reserve(amount);
            }
            else if (attr < QGL::CustomVertex0)
            {
                d->textures[d->key[field]].reserve(amount);
            }
            else
            {
                d->attributes[d->key[field]].reserve(amount);
            }
        }
    }
}

/*!
    Draws this geometry on the \a painter, from \a start for \a count elements
    in \a mode.  The drawing \a mode is by default QGL::Triangles.  This function
    Also calls the upload() method to ensure that the geometry is resident on
    the graphics hardware if appropriate.

    If the geometry is a point or line, then the \a drawWidth value specified the
    width/size of the line/point.
*/
void QGeometryData::draw(QGLPainter *painter, int start, int count, GLenum mode, qreal drawWidth)
{
    if (d && d->indices.size() && d->count)
    {
        upload();
        painter->clearAttributes();
        if (mode==QGL::Points) {
#if !defined(QT_OPENGL_ES_2)
            ::glPointSize(drawWidth);
#endif
        } else if (mode==QGL::LineStrip || mode == QGL::Lines) {
            ::glLineWidth(drawWidth);
        }
        painter->setVertexBundle(d->vertexBundle);
        if (count == 0)
            count = d->indexBuffer.indexCount();
        painter->draw(QGL::DrawingMode(mode), d->indexBuffer, start, count);
    }
}

/*!
    Uploads this geometry data to the graphics hardware if appropriate.  If the
    data is already uploaded and has not been modified since it was last
    uploaded, then this function does nothing.

    If the bufferStrategy() does not specify QGL::BufferIfPossible then this
    function does nothing.

    If the data was successfully uploaded, and the bufferStrategy() does not
    specify QGL::KeepClientData then the data will be removed with a call to
    the clear() function.

    If the data was successfully uploaded, on this call or previously, then this
    function will return true.  Otherwise it returns false.
*/
bool QGeometryData::upload()
{
    bool vboUploaded = false;
    bool iboUploaded = false;

    if (!d)
        return false;
    if (!d->modified)
        return d->vertexBundle.isUploaded() && d->indexBuffer.isUploaded();

    check();

    // Need to recreate the buffers from the modified data.
    d->vertexBundle = QGLVertexBundle();
    d->indexBuffer = QGLIndexBuffer();

    // Copy the geometry data to the vertex buffer.
    const quint32 mask = 0x01;
    quint32 fields = d->fields;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (!(mask & fields))
            continue;
        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
        if (attr == QGL::Position)
            d->vertexBundle.addAttribute(attr, d->vertices);
        else if (attr == QGL::Normal)
            d->vertexBundle.addAttribute(attr, d->normals);
        else if (attr == QGL::Color)
            d->vertexBundle.addAttribute(attr, d->colors);
        else if (attr < QGL::CustomVertex0)
            d->vertexBundle.addAttribute(attr, d->textures.at(d->key[field]));
        else
            d->vertexBundle.addAttribute(attr, d->attributes.at(d->key[field]));
    }

    // Upload the buffer if requested, otherwise keep it client-side.
    // Note: QGLVertexBundle will act as a client-side buffer if not uploaded.
    if ((d->bufferStrategy & BufferIfPossible) != 0)
    {
        if (d->vertexBundle.upload())
            vboUploaded = true;
    }

    // Copy the geometry data to the index buffer and upload if requested.
    d->indexBuffer.setIndexes(d->indices);
    if ((d->bufferStrategy & BufferIfPossible) != 0)
    {
        if (d->indexBuffer.upload())
            iboUploaded = true;
    }

    d->modified = false;

    if (!(d->bufferStrategy & KeepClientData) && vboUploaded && iboUploaded)
        clear();

    return vboUploaded && iboUploaded;
}

/*!
    Sets the buffer \a strategy for this geometry.

    \sa bufferStrategy()
*/
void QGeometryData::setBufferStrategy(QGeometryData::BufferStrategy strategy)
{
    if (!d || d->bufferStrategy != strategy)
    {
        create();
        d->modified = true;
        d->bufferStrategy = strategy;
    }
}

/*!
    Returns the buffer strategy for this geometry.  The default is
    \c{QGL::BufferIfPossible | QGL::KeepClientData}.

    \sa setBufferStrategy()
*/
QGeometryData::BufferStrategy QGeometryData::bufferStrategy() const
{
    if (d)
        return d->bufferStrategy;
    return InvalidStrategy;
}

/*!
    Returns a reference to the vertex buffer for this geometry.

    \sa indexBuffer()
*/
QGLVertexBundle QGeometryData::vertexBundle() const
{
    return d->vertexBundle;
}

/*!
    Returns a reference to the index buffer for this geometry.

    \sa vertexBundle()
*/
QGLIndexBuffer QGeometryData::indexBuffer() const
{
    return d->indexBuffer;
}

/*!
    Appends \a index to the vertex index array.

    \sa appendIndices(), indices()
*/
void QGeometryData::appendIndex(int index)
{
    create();
    d->modified = true;
    d->indices.append(index);
}

/*!
    Appends \a index1, \a index2, and \a index3 to the geometry's
    index array.

    \sa appendIndex(), indices()
*/
void QGeometryData::appendIndices(int index1, int index2, int index3)
{
    create();
    d->modified = true;
    d->indices.append(index1, index2, index3);
}

/*!
    Returns the index array that was created by appendIndex().

    \sa appendIndex(), appendIndices()
*/
QGL::IndexArray QGeometryData::indices() const
{
    if (d)
        return d->indices;
    else
        return QGL::IndexArray();
}

/*!
    Appends the \a indices to the geometry's index array.
*/
void QGeometryData::appendIndices(const QGL::IndexArray &indices)
{
    create();
    d->modified = true;
    d->indices.append(indices);
}

/*!
    Append the point \a v0 to this geometry data as a position field.
*/
void QGeometryData::appendVertex(const QVector3D &v0)
{
    create();
    d->modified = true;
    enableField(QGL::Position);
    d->vertices.append(v0);
    if (d->boxValid)
        d->bb.unite(v0);
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0 and \a v1 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1)
{
    create();
    d->modified = true;
    enableField(QGL::Position);
    d->vertices.append(v0, v1);
    if (d->boxValid)
    {
        d->bb.unite(v0);
        d->bb.unite(v1);
    }
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0, \a v1 and \a v2 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2)
{
    create();
    d->modified = true;
    enableField(QGL::Position);
    d->vertices.append(v0, v1, v2);
    if (d->boxValid)
    {
        d->bb.unite(v0);
        d->bb.unite(v1);
        d->bb.unite(v2);
    }
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the points \a v0, \a v1, \a v2 and \a v3 to this geometry data as position fields.
*/
void QGeometryData::appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2, const QVector3D &v3)
{
    create();
    d->modified = true;
    enableField(QGL::Position);
    d->vertices.append(v0, v1, v2, v3);
    if (d->boxValid)
    {
        d->bb.unite(v0);
        d->bb.unite(v1);
        d->bb.unite(v2);
        d->bb.unite(v3);
    }
    d->count = qMax(d->count, d->vertices.count());
}

/*!
    Append the float \a a0 to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->attributes[d->key[field]].append(a0);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the float \a a0 and \a a1 to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the floats \a a0, \a a1 and \a a2 to this geometry data, as attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, float a2, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1, a2);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the floats \a a0, \a a1, \a a2 and \a a3 to this geometry data, as attribute \a field.
*/
void QGeometryData::appendAttribute(float a0, float a1, float a2, float a3, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->attributes[d->key[field]].append(a0, a1, a2, a3);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the 2D point \a a to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVector2D &a, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    if (d->attributes.at(d->key[field]).isEmpty())
        d->attributes[d->key[field]].setElementType(QCustomDataArray::Vector2D);
    d->attributes[d->key[field]].append(a);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the 3D point \a v to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVector3D &v, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    if (d->attributes.at(d->key[field]).isEmpty())
        d->attributes[d->key[field]].setElementType(QCustomDataArray::Vector3D);
    d->attributes[d->key[field]].append(v);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the variant value \a a to this geometry data, as an attribute \a field.
*/
void QGeometryData::appendAttribute(const QVariant &a, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    if (d->attributes.at(d->key[field]).isEmpty())
    {
        // floats and doubles get handled "automatically" - float is default
        if (a.type() == QVariant::Vector2D)
            d->attributes[d->key[field]].setElementType(QCustomDataArray::Vector2D);
        else if (a.type() == QVariant::Vector3D)
            d->attributes[d->key[field]].setElementType(QCustomDataArray::Vector3D);
        else if (a.type() == QVariant::Vector4D)
            d->attributes[d->key[field]].setElementType(QCustomDataArray::Vector4D);
        else if (a.type() == QVariant::Color)
            d->attributes[d->key[field]].setElementType(QCustomDataArray::Color);
        else
            Q_ASSERT_X(false, "QGeometryData::appendAttribute", "bad type");
    }
    d->attributes[d->key[field]].append(a);
    d->count = qMax(d->count, d->attributes[d->key[field]].count());
}

/*!
    Append the vector \a n0 to this geometry data, as a lighting normal.
*/
void QGeometryData::appendNormal(const QVector3D &n0)
{
    create();
    d->modified = true;
    enableField(QGL::Normal);
    d->normals.append(n0);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0 and \a n1 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1)
{
    create();
    d->modified = true;
    enableField(QGL::Normal);
    d->normals.append(n0, n1);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0, \a n1 and \a n2 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2)
{
    create();
    d->modified = true;
    enableField(QGL::Normal);
    d->normals.append(n0, n1, n2);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the vectors \a n0, \a n1, \a n2 and \a n3 to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2, const QVector3D &n3)
{
    create();
    d->modified = true;
    enableField(QGL::Normal);
    d->normals.append(n0, n1, n2, n3);
    d->count = qMax(d->count, d->normals.count());
}

/*!
    Append the point \a t0 to this geometry data, as an texture \a field.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->textures[d->key[field]].append(t0);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0 and \a t1 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->textures[d->key[field]].append(t0, t1);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0, \a t1 and \a t2 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->textures[d->key[field]].append(t0, t1, t2);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the points \a t0, \a t1, \a t2 and \a t3 to this geometry data, as texture \a{field}s.
*/
void QGeometryData::appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, const QVector2D &t3, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    enableField(field);
    d->textures[d->key[field]].append(t0, t1, t2, t3);
    d->count = qMax(d->count, d->textures[d->key[field]].count());
}

/*!
    Append the color \a c0 to this geometry data, as an color field.
*/
void QGeometryData::appendColor(const QColor4ub &c0)
{
    create();
    d->modified = true;
    enableField(QGL::Color);
    d->colors.append(c0);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0 and \a c1 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4ub &c0, const QColor4ub &c1)
{
    create();
    d->modified = true;
    enableField(QGL::Color);
    d->colors.append(c0, c1);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0, \a c1 and \a c2 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4ub &c0, const QColor4ub &c1, const QColor4ub &c2)
{
    create();
    d->modified = true;
    enableField(QGL::Color);
    d->colors.append(c0, c1, c2);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the color \a c0, \a c1, \a c2  and \a c3 to this geometry data, as color fields.
*/
void QGeometryData::appendColor(const QColor4ub &c0, const QColor4ub &c1, const QColor4ub &c2, const QColor4ub &c3)
{
    create();
    d->modified = true;
    enableField(QGL::Color);
    d->colors.append(c0, c1, c2, c3);
    d->count = qMax(d->count, d->colors.count());
}

/*!
    Append the points in \a ary to this geometry data as position fields.
*/
void QGeometryData::appendVertexArray(const QVector3DArray &ary)
{
    if (ary.count())
    {
        create();
        d->modified = true;
        d->boxValid = false;
        enableField(QGL::Position);
        d->vertices.append(ary);
        d->count = qMax(d->count, d->vertices.count());
    }
}

/*!
    Append the points in \a ary to this geometry data, as an attribute \a field entries.
*/
void QGeometryData::appendAttributeArray(const QCustomDataArray &ary, QGL::VertexAttribute field)
{
    if (ary.count())
    {
        create();
        d->modified = true;
        enableField(field);
        d->attributes[d->key[field]].append(ary);
        d->count = qMax(d->count, d->attributes[d->key[field]].count());
    }
}

/*!
    Append the vectors in \a ary to this geometry data, as lighting normals.
*/
void QGeometryData::appendNormalArray(const QVector3DArray &ary)
{
    if (ary.count())
    {
        create();
        d->modified = true;
        enableField(QGL::Normal);
        d->normals.append(ary);
        d->count = qMax(d->count, d->normals.count());
    }
}

/*!
    Append the 2D points in \a ary to this geometry data, as texture \a field entries.
*/
void QGeometryData::appendTexCoordArray(const QVector2DArray &ary, QGL::VertexAttribute field)
{
    if (ary.count())
    {
        create();
        d->modified = true;
        enableField(field);
        d->textures[d->key[field]].append(ary);
        d->count = qMax(d->count, d->textures[d->key[field]].count());
    }
}

/*!
    Append the colors in \a ary to this geometry data, as color fields.
*/
void QGeometryData::appendColorArray(const QArray<QColor4ub> &ary)
{
    if (ary.count())
    {
        create();
        d->modified = true;
        enableField(QGL::Color);
        d->colors.append(ary);
        d->count = qMax(d->count, d->colors.count());
    }
}

/*!
    Returns a modifiable reference to the vertex data at index \a i.
*/
QVector3D &QGeometryData::vertex(int i)
{
    create();
    d->modified = true;
    d->boxValid = false;
    return d->vertices[i];
}

/*!
    Returns a copy of the vertex position data.
*/
QVector3DArray QGeometryData::vertices() const
{
    if (d)
        return d->vertices;
    return QArray<QVector3D>();
}

/*!
    \internal
    Returns a pointer to the vertex data.
*/
const QVector3DArray *QGeometryData::vertexData() const
{
    if (d)
        return &d->vertices;
    return 0;
}


/*!
    Returns a non-modifiable reference to the vertex position data at index \a i.
*/
const QVector3D &QGeometryData::vertexAt(int i) const
{
    Q_ASSERT(hasField(QGL::Position));
    return d->vertices.at(i);
}

/*!
    Returns a modifiable reference to the normal data at index \a i.
*/
QVector3D &QGeometryData::normal(int i)
{
    create();
    d->modified = true;
    return d->normals[i];
}

/*!
    Returns a non-modifiable reference to the normal data at index \a i.
*/
const QVector3D &QGeometryData::normalAt(int i) const
{
    Q_ASSERT(hasField(QGL::Normal));
    return d->normals.at(i);
}

/*!
    Returns a copy of the lighting normal data.
*/
QVector3DArray QGeometryData::normals() const
{
    if (d)
        return d->normals;
    return QArray<QVector3D>();
}

/*!
    Returns a modifiable reference to the color data at index \a i.
*/
QColor4ub &QGeometryData::color(int i)
{
    create();
    d->modified = true;
    return d->colors[i];
}

/*!
    Returns a non-modifiable reference to the color data at index \a i.
*/
const QColor4ub &QGeometryData::colorAt(int i) const
{
    Q_ASSERT(hasField(QGL::Color));
    return d->colors.at(i);
}

/*!
    Returns a copy of the color data.
*/
QArray<QColor4ub> QGeometryData::colors() const
{
    if (d)
        return d->colors;
    return QArray<QColor4ub>();
}

/*!
    Returns a modifiable reference to the \a field texture coordinate data at index \a i.
*/
QVector2D &QGeometryData::texCoord(int i, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    return d->textures[d->key[field]][i];
}

/*!
    Returns a copy of the \a field texture coordinate data.
*/
QVector2DArray QGeometryData::texCoords(QGL::VertexAttribute field) const
{
    return hasField(field) ? d->textures.at(d->key[field]) : QVector2DArray();
}

/*!
    Returns a non-modifiable reference to the texture coordinate data at index \a i for \a field.
*/
const QVector2D &QGeometryData::texCoordAt(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->textures.at(d->key[field]).at(i);
}

/*!
    Returns a modifiable reference to the float \a field attribute data at index \a i.
*/
float &QGeometryData::floatAttribute(int i, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Float);
    return ary.m_array[i];
}

/*!
    Returns a modifiable reference to the 2D vector \a field attribute data at index \a i.
*/
QVector2D &QGeometryData::vector2DAttribute(int i, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Vector2D);
    float *data = ary.m_array.data();
    QVector2D *v = reinterpret_cast<QVector2D*>(data + i*2);
    return *v;
}

/*!
    Returns a modifiable reference to the 3D vector \a field attribute data at index \a i.
*/
QVector3D &QGeometryData::vector3DAttribute(int i, QGL::VertexAttribute field)
{
    create();
    d->modified = true;
    QCustomDataArray &ary = d->attributes[d->key[field]];
    Q_ASSERT(ary.elementType() == QCustomDataArray::Vector3D);
    float *data = ary.m_array.data();
    QVector3D *v = reinterpret_cast<QVector3D*>(data + i*2);
    return *v;
}

/*!
    Returns a copy of the \a field attribute data.
*/
QCustomDataArray QGeometryData::attributes(QGL::VertexAttribute field) const
{
    return hasField(field) ? d->attributes.at(d->key[field]) : QCustomDataArray();
}

/*!
    Returns a copy of the float \a field attribute data at index \a i.
*/
float QGeometryData::floatAttributeAt(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).floatAt(i);
}

/*!
    Returns a copy of the 2D vector \a field attribute data at index \a i.
*/
QVector2D QGeometryData::vector2DAttributeAt(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).vector2DAt(i);
}

/*!
    Returns a copy of the 3D vector \a field attribute data at index \a i.
*/
QVector3D QGeometryData::vector3DAttributeAt(int i, QGL::VertexAttribute field) const
{
    Q_ASSERT(hasField(field));
    return d->attributes.at(d->key[field]).vector3DAt(i);
}

/*!
    Returns the attribute value for the \a field, suitable for passing
    to QGLPainter.

    \sa QGLPainter::setVertexAttribute()
*/
QGLAttributeValue QGeometryData::attributeValue(QGL::VertexAttribute field) const
{
    if (hasField(field))
    {
        if (field < QGL::TextureCoord0)
        {
            if (field == QGL::Position)
                return QGLAttributeValue(d->vertices);
            else if (field == QGL::Normal)
                return QGLAttributeValue(d->normals);
            else if (field == QGL::Color)
                return QGLAttributeValue(d->colors);
        }
        else
        {
            if (field < QGL::CustomVertex0)
                return QGLAttributeValue(d->textures.at(d->key[field]));
            else
                return QGLAttributeValue(d->attributes.at(d->key[field]));
        }
    }
    return QGLAttributeValue();
}

/*!
    Returns true if this geometry has the field corresponding to \a attr.  Note
    that it is still possible for no data to have been added for that field.
*/
bool QGeometryData::hasField(QGL::VertexAttribute attr) const
{
    if (d)
        return d->key[attr] != -1;
    return false;
}

/*!
    Enables this geometry to contain data of type \a field.  Generally it is
    not necessary to call this function since it is called by all the append
    functions.
*/
void QGeometryData::enableField(QGL::VertexAttribute field)
{
    if (d && d->key[field] != -1)
        return;
    create();
    d->modified = true;
    Q_ASSERT(field < d->ATTR_CNT); // don't expand that enum too much
    d->fields |= (1 << field);
    switch (field)
    {
    case QGL::Position:
        d->key[QGL::Position] = 0;
        d->size[QGL::Position] = 3;
        if (d->reserved > 0)
            d->vertices.reserve(d->reserved);
        break;
    case QGL::Normal:
        d->key[QGL::Normal] = 1;
        d->size[QGL::Normal] = 3;
        if (d->reserved > 0)
            d->normals.reserve(d->reserved);
        break;
    case QGL::Color:
        d->key[QGL::Color] = 2;
        d->size[QGL::Color] = 1;
        if (d->reserved > 0)
            d->colors.reserve(d->reserved);
        break;
    case QGL::TextureCoord0:
    case QGL::TextureCoord1:
    case QGL::TextureCoord2:
        d->textures.append(QVector2DArray());
        d->key[field] = d->textures.count() - 1;
        d->size[field] = 2;
        if (d->reserved > 0)
            d->textures[d->key[field]].reserve(d->reserved);
        break;
    default:
        // Custom and User vertex attributes.
        d->attributes.append(QCustomDataArray());
        d->key[field] = d->attributes.count() - 1;
        d->size[field] = d->attributes.at(d->key[field]).elementSize();
        if (d->reserved > 0)
            d->attributes[d->key[field]].reserve(d->reserved);
        break;
    }
}

/*!
    Return a bit-mask of the supported fields in this geometry.  The
    QGL::VertexAttribute enum can be recovered from this bit-mask by
    \code
    quint32 fields = fields();
    QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(fields);
    \endcode
*/
quint32 QGeometryData::fields() const
{
    if (d)
        return d->fields;
    return 0;
}

/*!
    Returns the count of logical vertices stored.  This is effectively
    the max() of QArray::count() over all of the enabled data types.
*/
int QGeometryData::count() const
{
    if (d)
        return d->count;
    return 0;
}

/*!
    Returns the count of data stored in \a field.  This will always be at
    most count(), but could be less.
*/
int QGeometryData::count(QGL::VertexAttribute field) const
{
    int result = 0;
    if (d && (QGL::fieldMask(field) & d->fields))
    {
        if (field < QGL::TextureCoord0)
        {
            if (field == QGL::Position)
                result = d->vertices.count();
            else if (field == QGL::Normal)
                result = d->normals.count();
            else
                result = d->colors.count();
        }
        else if (field < QGL::CustomVertex0)
        {
            result = d->textures[d->key[field]].count();
        }
        else
        {
            result = d->attributes[d->key[field]].count();
        }
    }
    return result;
}

/*!
    Returns true if this geometry is identical to the \a other; and false otherwise.
*/
bool QGeometryData::operator==(const QGeometryData &other) const
{
    bool isEqual = false;
    if (d)
    {
        if (d == other.d)
        {
            isEqual = true;
        }
        else
        {
            if (other.d && d->fields == other.d->fields && d->count == other.d->count)
            {
                const quint32 mask = 0x01;
                quint32 fields = d->fields;
                isEqual = true;
                for (int field = 0; fields && isEqual; ++field, fields >>= 1)
                {
                    if (mask & fields)
                    {
                        QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
                        if (attr < QGL::TextureCoord0)
                        {
                            if (attr == QGL::Position)
                                isEqual = (d->vertices == other.d->vertices);
                            else if (attr == QGL::Normal)
                                isEqual = (d->normals == other.d->normals);
                            else  // colors
                                isEqual = (d->colors == other.d->colors);
                        }
                        else if (attr < QGL::CustomVertex0)
                        {
                            isEqual = (d->textures.at(d->key[attr]) == other.d->textures.at(d->key[attr]));
                        }
                        else
                        {
                            QArray<float> me = d->attributes.at(d->key[attr]).toFloatArray();
                            QArray<float> him = other.d->attributes.at(d->key[attr]).toFloatArray();
                            isEqual = (me == him);
                        }
                    }
                }
            }
        }
    }
    else
    {
        isEqual = other.isNull();
    }
    return isEqual;
}

/*!
    Returns true if this geometry is empty - that is it contains no vertices
    or other data - and returns false otherwise.  If an existing geometry has
    been made empty by a call to clear() then this will be true (but isNull()
    will be false).

    \sa isNull()
*/
bool QGeometryData::isEmpty() const
{
    bool empty = true;
    if (d)
        empty = d->count == 0;
    return empty;
}

/*!
    Returns true if this geometry is uninitialized - that is it contains no
    internal data structures.  A newly constructed QGeometryData object is
    null until some data is added or changed.

    \sa isEmpty()
*/
bool QGeometryData::isNull() const
{
    return d == NULL;
}

/*!
    Returns the number of index values stored in this geometry data.

    This value is exactly the same as indices().size() (but does not
    incur the copy).
*/
int QGeometryData::indexCount() const
{
    if (d)
        return d->indices.size();
    return 0;
}

void QGeometryData::create()
{
    if (!d) // lazy creation of data block
    {
        d = new QGeometryDataPrivate;
        d->ref.ref();
    }
}

/*!
    Force this geometry to ensure it has its own unshared internal data
    block, making a copy in the case that it is currently shared.
*/
void QGeometryData::detach()
{
    create();
    if (d->ref > 1)  // being shared, must detach
    {
        QGeometryDataPrivate *temp = d->clone();
        d->ref.deref();
        d = temp;
        d->ref.ref();
    }
}

/*!
    \fn quint64 QGeometryData::id() const
    Return an opaque value that can be used to identify which data block is
    being used by this QGeometryData instance.  See the class documentation
    relating to explicit sharing.
*/

#ifndef QT_NO_DEBUG
void QGeometryData::check() const
{
    if (!d)
        return;
    const quint32 mask = 0x01;
    quint32 fields = d->fields;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                {
                    if (d->vertices.count() < d->count)
                        qWarning("QGeometryData - expected %d vertices, only %d found!",
                                 d->count, d->vertices.count());
                }
                else if (attr == QGL::Normal)
                {
                    if (d->normals.count() < d->count)
                        qWarning("QGeometryData - expected %d normals, only %d found!",
                                 d->count, d->normals.count());
                }
                else
                {
                    if (d->colors.count() < d->count)
                        qWarning("QGeometryData - expected %d colors, only %d found!",
                                 d->count, d->colors.count());
                }
            }
            else if (attr < QGL::CustomVertex0)
            {
                if (d->textures.at(d->key[field]).count() < d->count)
                    qWarning("QGeometryData - expected %d texture coordinates for"
                             "QGL::TextureCoord%d, only %d found!",
                             d->count, field - QGL::TextureCoord0, d->textures.at(d->key[field]).count());
            }
            else
            {
                if (d->attributes.at(d->key[field]).count() < d->count)
                    qWarning("QGeometryData - expected %d attributes for"
                             "QGL::CustomVertex%d, only %d found!",
                             d->count, field - QGL::CustomVertex0, d->attributes.at(d->key[field]).count());
            }
        }
    }
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGeometryData &vertices)
{
    dbg << "QGeometryData" << &vertices << " size:" << vertices.count()
#ifndef QT_NO_DEBUG
        << "data block id:" << vertices.id()
#endif
           ;
    quint32 fields = vertices.fields();
    const quint32 mask = 0x01;
    for (int field = 0; fields; ++field, fields >>= 1)
    {
        if (mask & fields)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(field);
            if (attr < QGL::TextureCoord0)
            {
                if (attr == QGL::Position)
                {
                    dbg << "    vertices:" << vertices.count(attr);
                    dbg << vertices.vertices();
                }
                else if (attr == QGL::Normal)
                {
                    dbg << "    normals:" << vertices.count(attr);
                    dbg << vertices.normals();
                }
                else
                {
                    dbg << "    colors:" << vertices.count(attr);
                    dbg << vertices.colors();
                }
            }
            else if (attr < QGL::CustomVertex0)
            {
                dbg << "    textures:" << (attr - QGL::TextureCoord0) << vertices.count(attr);
                dbg << vertices.texCoords(attr);
            }
            else
            {
                dbg << "    custom:" << (attr - QGL::CustomVertex0) << vertices.count(attr);
                dbg << vertices.texCoords(attr);
            }
        }
    }
    if (vertices.indexCount() > 0)
    {
        dbg << "    indices:" << vertices.indices();
    }
    return dbg;
}
#endif
