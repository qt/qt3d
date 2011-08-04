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

#include "qglsection_p.h"
#include "qglbuilder_p.h"
#include "qarray.h"
#include "qvector_utils_p.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpointer.h>
#include <QtCore/qmap.h>
#include <QtCore/qbitarray.h>

#include <limits.h>

/*!
    \internal
    \class QGLSection
    \brief The QGLSection class clusters like geometry in a QGLBuilder.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLSection instances partition a QGLBuilder into related sections,
    while the builder is being initialized with geometry data.

    Once the builder is initialized, and geometry building is complete
    the QGLSection instances are destroyed and the data is uploaded to the
    graphics hardware.

    The QGLSection class is a work horse for the QGLBuilder, and it
    takes care of automatically managing vertex data.  As such
    for usual use cases, its functionality will not need to be referenced
    directly.  For low-level access to geometry, QGLSection provides a
    range of accessors to reference geometry data during scene building.

    Within a section, incoming geometry data will be coalesced and
    indexes created to reference the fewest possible copies of the vertex
    data.  For example, in smooth geometry all copies of a vertex are
    coalesced into one, and referenced by indices - except in the case
    where texture data forms a \i seam and a copy must be created to carry
    the two texture coordinates of the seam.

    This is handled automatically by QGLSection, to pack data into the
    smallest space possible thus improving cache coherence and performance.

    All the vertices in a QGLSection are treated with the same
    \l{QGL::Smoothing}{smoothing}, and have the same
    \l{QLogicalVertex::Type}{data types}.

    Each QGLSection references a contiguous range of vertices in a
    QGLBuilder.

    A QGLBuilder instance has the \l{QGLBuilder::newSection()}{newSection()}
    function which creates a new QGLSection to reference its data.  Use this
    to construct new QGLSection instances, or alternatively construct
    a new QGLSection() and pass a non-null QGLBuilder pointer.

    These functions all return QVector values. QVector instances are
    implicitly shared, thus the copies are inexpensive unless a
    non-const function is called on them, triggering a copy-on-write.

    Generally for adding geometry, use append().  This function simply
    calls virtual protected functions appendSmooth() (for smoothed vertices)
    and appendFaceted() (for faceted vertices).  See QGLBuilder for a
    discussion of smoothing.
*/

// allow QVector3D's to be stored in a QMap
inline bool operator<(const QVector3D &a, const QVector3D &b)
{
    if (qFskCompare(a.x(), b.x()))
    {
        if (qFskCompare(a.y(), b.y()))
        {
            if (qFskCompare(a.z(), b.z()))
            {
                return false;  // equal so not less-than
            }
            else
            {
                return a.z() < b.z();
            }
        }
        else
        {
            return a.y() < b.y();
        }
    }
    else
    {
        return a.x() < b.x();
    }
}

static inline bool qSameDirection(const QVector3D &a , const QVector3D &b)
{
    bool res = false;
    if (!a.isNull() && !b.isNull())
    {
        float dot = QVector3D::dotProduct(a, b);
        res = qFskCompare((qreal)dot, a.length() * b.length());
    }
    return res;
}

class QGLSectionPrivate
{
public:
    QGLSectionPrivate(const QVector3DArray *ary)
        : vec_data(ary)
        , it(vec_map.end())
        , map_threshold(5)
        , number_mapped(0)
        , start_ptr(-1)
        , end_ptr(-1)
    {
        normIndices.fill(-1, 32);
    }

    ~QGLSectionPrivate() {}

    bool normalAccumulated(int index, const QVector3D &norm) const
    {
        if (index >= normIndices.size())
            return false;
        int ptr = normIndices.at(index);
        while (ptr != -1)
        {
            int val_ptr = normPtrs.at(ptr);
            //if (normValues.at(val_ptr) == norm)
            if (qSameDirection(normValues.at(val_ptr), norm))
                return true;
            ptr = normPtrs.at(ptr+1);
        }
        return false;
    }

    void accumulateNormal(int index, const QVector3D &norm)
    {
        int new_norm_index = normValues.size();
        normValues.append(norm);
        if (normIndices.size() <= index)
        {
            int old_size = normIndices.size();
            normIndices.extend(32);
            for (int i = old_size; i < normIndices.size(); ++i)
                normIndices[i] = -1;
        }
        int new_norm_ptr = normPtrs.size();
        normPtrs.append(new_norm_index);  // even ptrs point to vector value
        normPtrs.append(-1);              // odd ptrs point to next in normPtr linked list
        if (normIndices.at(index) == -1)
        {
            normIndices[index] = new_norm_ptr;
        }
        else
        {
            int norm_ptr = normIndices.at(index);
            while (normPtrs.at(norm_ptr + 1) != -1)
            {
                norm_ptr = normPtrs.at(norm_ptr + 1);
            }
            normPtrs[norm_ptr+1] = new_norm_ptr;
        }
    }

    void mapVertex(const QVector3D &v, int ix)
    {
        Q_UNUSED(ix);
        Q_UNUSED(v);
        static bool seeded = false;
        if (!seeded)
            qsrand(31415);
        Q_ASSERT(vec_data->at(ix) == v);
        if ((vec_data->size() - number_mapped) > map_threshold)
        {
            int to_map = vec_data->size() - number_mapped;
            QArray<int, 100> shuffle(to_map, -1);
            for (int i = number_mapped, k = 0; i < vec_data->size(); ++i, ++k)
                shuffle[k] = i;
            for (int n = to_map; n > 1; --n)
            {
                int k = qrand() % n;
                int tmp = shuffle[k];
                shuffle[k] = shuffle[n - 1];
                shuffle[n - 1] = tmp;
            }
            for (int i = 0; i < to_map; ++i)
                vec_map.insertMulti(vec_data->at(shuffle.at(i)), shuffle.at(i));
            number_mapped += to_map;
        }
    }

    int nextIndex()
    {
        int result = -1;
        if (end_ptr != -1)
        {
            // first look through the unmapped items
            while (start_ptr <= end_ptr && result == -1)
            {
                // search from the end and beginning, favouring the end - most often
                // its in the last few we added, sometimes in the first ones
                if (qFskCompare(vec_data->at(end_ptr--), target))
                    result = end_ptr+1;
                else if (start_ptr <= end_ptr && qFskCompare(vec_data->at(end_ptr--), target))
                    result = end_ptr+1;
                else if (start_ptr <= end_ptr && qFskCompare(vec_data->at(start_ptr++), target))
                    result = start_ptr-1;
            }
            // if that found nothing, have a look at the map
            if (result == -1)
            {
                start_ptr = -1;
                end_ptr = -1;
                it = vec_map.constEnd();
                if (vec_map.size() > 0)
                    it = vec_map.find(target);
            }
        }
        if (it != vec_map.constEnd())
        {
            // if there was something in the map see if its still on target
            if (qFskCompare(it.key(), target))
            {
                result = it.value();
                ++it;  // increment to find more insertMulti instances
            }
            else
            {
                // not on target - flag that we're done here
                it = vec_map.constEnd();
            }
        }
        return result;
    }

    int findVertex(const QVector3D &v)
    {
        end_ptr = vec_data->size() - 1;   // last one not in QMap
        start_ptr = number_mapped;        // first one not in QMap
        target = v;
        return nextIndex();
    }

    // mapper
    int index;
    QVector3D target;
    const QVector3DArray *vec_data;
    QMap<QVector3D, int> vec_map;
    QMap<int, int> index_map;
    QMap<QVector3D,int>::const_iterator it;
    int map_threshold;   // if more than this is unmapped, do a mapping run
    int number_mapped;    // how many vertices have been mapped
    int start_ptr;
    int end_ptr;

    QArray<int, 32> normIndices;
    QArray<int, 32> normPtrs;
    QArray<QVector3D, 32> normValues;

    QList<QGLSceneNode*> nodes;
};

/*!
    \internal
    Construct a new QGLSection on \a builder, and with smoothing \a s.
    By default the smoothing is QGL::Smooth.

    See QGLBuilder for a discussion of smoothing.

    The pointer \a list must be non-null, and in debug mode, unless QT_NO_DEBUG is
    defined, this function will assert if \a list is null.

    The following lines of code have identical effect:
    \code
    QGLSection *s = myDisplayList->newSection(QGL::Faceted);
    QGLSection *s2 = new QGLSection(myDisplayList, QGL::Faceted);
    \endcode
*/
QGLSection::QGLSection(QGLBuilder *builder, QGL::Smoothing s)
    : m_smoothing(s)
    , d(0)
{
    Q_ASSERT(builder);
    enableField(QGL::Position);
    Q_ASSERT(vertexData());
    d = new QGLSectionPrivate(vertexData());
    builder->addSection(this);
}

/*!
    \internal
    Destroy this QGLSection, recovering any resources.
*/
QGLSection::~QGLSection()
{
    delete d;
}

/*!
    \internal
    Reserve capacity for \a amount items.  This may avoid realloc
    overhead when a large number of items will be appended.
*/
void QGLSection::reserve(int amount)
{
    QGeometryData::reserve(amount);
    d->normIndices.reserve(amount);
    d->normPtrs.reserve(amount * 2);
    d->normValues.reserve(amount);
}

/*!
    \internal
    Adds the logical vertices \a a, \a b and \c to this section.  All
    should have the same fields.  This function is exactly equivalent to
    \code
        append(a); append(b); append(c);
    \endcode

    \sa appendSmooth(), appendFaceted()
*/
void QGLSection::append(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
{
    Q_ASSERT(a.fields() == b.fields() && b.fields() == c.fields());
    if (!a.hasField(QGL::Normal))
    {
        appendFaceted(a, b, c);
    }
    else
    {
        if (m_smoothing == QGL::Smooth)
            appendSmooth(a, b, c);
        else
            appendFaceted(a, b, c);
    }
}

/*!
    \internal
    Adds the logical vertex \a lv to this section.

    Otherwise, if the \a lv does have a lighting normal; then the
    vertex processing depends on the smoothing property of this section.
    If this section has smoothing QGL::Smooth, then the append will be done
    by calling appendSmooth(); or if this section has smoothing QGL::Faceted,
    then the append will be done by calling appendFaceted().

    \sa appendSmooth(), appendFaceted()
*/
void QGLSection::append(const QLogicalVertex &lv)
{
    if (!lv.hasField(QGL::Normal))
    {
        appendFaceted(lv);
    }
    else
    {
        if (m_smoothing == QGL::Smooth)
            appendSmooth(lv);
        else
            appendFaceted(lv);
    }
}

static bool qCompareByAttributes(const QLogicalVertex &a, const QLogicalVertex &b)
{
    static const quint32 ATTRS_AND_TEXTURES = (0xFFFFFFFF << QGL::TextureCoord0);
    quint32 af = a.fields() & ATTRS_AND_TEXTURES;
    quint32 bf = b.fields() & ATTRS_AND_TEXTURES;
    if (af != bf)
        return false;
    quint32 flds = af | bf;
    const quint32 mask = 0x01;
    flds >>= QGL::TextureCoord0;
    for (int i = QGL::TextureCoord0; flds; ++i, flds >>= 1)
    {
        if (flds & mask)
        {
            QGL::VertexAttribute attr = static_cast<QGL::VertexAttribute>(i);
            if (attr < QGL::CustomVertex0)
            {
                if (!qFskCompare(a.texCoord(attr), b.texCoord(attr)))
                    return false;
            }
            else
            {
                QVariant v1 = a.attribute(attr);
                QVariant v2 = b.attribute(attr);
                if (v1.type() == (QVariant::Type)QMetaType::Float)
                    return qFskCompare(v1.toFloat(), v2.toFloat());
                else if (v1.type() == QVariant::Vector2D)
                    return qFskCompare(qVariantValue<QVector2D>(v1), qVariantValue<QVector2D>(v2));
                else if (v1.type() == QVariant::Vector3D)
                    return qFskCompare(qVariantValue<QVector3D>(v1), qVariantValue<QVector3D>(v2));
                else
                    return v1 == v2;
            }
        }
    }
    return true;
}

int QGLSection::appendOne(const QLogicalVertex &lv)
{
#ifndef QT_NO_DEBUG_STREAM
    if (count() && lv.fields() != fields())
    {
        qDebug() << "Warning: adding" << lv << "fields:" << lv.fields()
                << "fields do not match existing:" << fields()
                << "create new section first?";
    }
#endif
    int index = appendVertex(lv);
    d->mapVertex(lv.vertex(), index);
    appendIndex(index);
    return index;
}

/*!
    \internal
    Adds the logical vertex \a lv to this section of a builder.

    Two QLogicalVertex instances a and b are treated as being duplicates for
    the purpose of smoothing, if \c{qFuzzyCompare(a.vertex(), b.vertex())} is
    true

    All duplicate occurrences of a vertex are coalesced, that is replaced
    by a GL index referencing the one copy.

    In order to draw \a lv as part of a smooth continuous surface, with
    no distinct edge, duplicates of vertex \a lv are coalesced into one
    (within this section) and the normal for that one set to the average of
    the incoming unique normals.

    The incoming vertex \a lv is not treated as a duplicate if \a lv has
    different texture coordinates or attributes.  This occurs for example
    in the case of a texture seam, where two different texture coordinates
    are required at the same point on the geometry.

    In that case a new duplicate vertex is added to carry the unique
    texture coordinates or attributes.  When new vertex copies are added in
    this way all copies receive the averaged normals.

    Call this function to add the vertices of a smooth face to the section
    of a builder, or use:

    \code
    myDisplayList->newSection(QGLBuilder::Smooth);
    myDisplayList->addTriangle(a, b, c);
    \endcode

    In smooth surfaces, the vertex and its normal is only sent to the
    graphics hardware once (not once per face), thus smooth geometry may
    consume fewer resources.

    \sa appendFaceted(), updateTexCoord(), QGLBuilder::newSection()
*/
void QGLSection::appendSmooth(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasField(QGL::Position));
    Q_ASSERT(lv.hasField(QGL::Normal));

    int found_index = d->findVertex(lv.vertex());
    bool coalesce = false;
    if (found_index == -1)
    {
        int newIndex = appendOne(lv);
        d->accumulateNormal(newIndex, lv.normal());
    }
    else
    {
        while (!coalesce && found_index != -1)
        {
            if (qCompareByAttributes(lv, logicalVertexAt(found_index)))
                coalesce = true;
            else
                found_index = d->nextIndex();
        }
        if (!coalesce)  // texture or attributes prevented coalesce
        {
            // new vert to carry tex/attrib data
            d->accumulateNormal(appendOne(lv), lv.normal());
        }
        else
        {
            appendIndex(found_index);
            while (found_index != -1)
            {
                if (!d->normalAccumulated(found_index, lv.normal()))
                {
                    normal(found_index) += lv.normal();
                    d->accumulateNormal(found_index, lv.normal());
                }
                found_index = d->nextIndex();
            }
        }
    }
}


void QGLSection::appendSmooth(const QLogicalVertex &lv, int index)
{
    Q_ASSERT(lv.hasField(QGL::Position));
    Q_ASSERT(lv.hasField(QGL::Normal));

    int found_index = -1;
    QMap<int, int>::const_iterator it = d->index_map.constFind(index);
    if (it != d->index_map.constEnd())
        found_index = it.value();
    if (found_index == -1)
    {
        int newIndex = appendVertex(lv);
        d->index_map.insert(index, newIndex);
        appendIndex(newIndex);
        d->accumulateNormal(newIndex, lv.normal());
    }
    else
    {
        appendIndex(found_index);
        if (!d->normalAccumulated(found_index, lv.normal()))
        {
            normal(found_index) += lv.normal();
            d->accumulateNormal(found_index, lv.normal());
        }
    }
}

/*!
    \internal
    Add the logical vertex \a lv to this section of a builder.

    The vertex will be drawn as a distinct edge, instead of just part of a
    continuous smooth surface.  To acheive this the vertex value of \a lv
    is duplicated for each unique normal in the current section.

    Note that duplication is only for unique normals: if a vertex is already
    present with a given normal it is coalesced and simply referenced by index.
    As for appendSmooth() vertices are not coalesced in this way if \a lv
    has a different texture coordinate or attribute than its duplicate.

    In faceted surfaces, the vertex is sent to the graphics hardware once for
    each normal it has, and thus may consume more resources.

    \sa appendSmooth(), updateTexCoord(), QGLBuilder::newSection()
*/
void QGLSection::appendFaceted(const QLogicalVertex &lv)
{
    Q_ASSERT(lv.hasField(QGL::Position));
    int found_index = d->findVertex(lv.vertex());
    bool coalesce = false;
    while (!coalesce && found_index != -1)
    {
        if (logicalVertexAt(found_index) == lv)
            coalesce = true;
        else
            found_index = d->nextIndex();
    }
    if (coalesce) // found
    {
        appendIndex(found_index);
    }
    else
    {
        appendOne(lv);
    }
}

/*!
    \internal
    Returns the current map threshold for this section.  The threshold is the
    point at which the section switches from using a plain QArray - with
    linear performance ie O(n) - to using a QMap - with approx O(log n).  These
    structures are used for looking up vertices during the index generation and
    normals calculation.

    The default value is 50.

    \sa setMapThreshold()
*/
int QGLSection::mapThreshold() const
{
    return d->map_threshold;
}

/*!
    \internal
    Sets the current map threshold to \a t for this section.

    \sa mapThreshold()
*/
void QGLSection::setMapThreshold(int t)
{
    d->map_threshold = t;
}

/*!
    \internal
    Returns a list of the QGLSceneNode instances associated with this section.
*/
QList<QGLSceneNode*> QGLSection::nodes() const
{
    return d->nodes;
}

/*!
    \internal
    Adds the \a node to the list of QGLSceneNode instances associated with
    this section.
*/
void QGLSection::addNode(QGLSceneNode *node)
{
    d->nodes.append(node);
}

/*!
    \internal
    Deletes the \a node from the list of QGLSceneNode instances associated
    with this section.  Returns true if the \a node was found, false
    otherwise.
*/
bool QGLSection::deleteNode(QGLSceneNode *node)
{
    int ix = d->nodes.indexOf(node);
    if (ix != -1)
    {
        d->nodes.removeAt(ix);
        return true;
    }
    return false;
}

#ifndef QT_NO_DEBUG_STREAM
/*!
    \internal
    Output a string representation of \a section to a debug stream \a dbg.
    \relates QGLSection
*/
QDebug operator<<(QDebug dbg, const QGLSection &section)
{
    dbg.space()
            << "QGLSection(" << &section
            << "- count:" << section.count()
            << "- smoothing mode:" << (section.smoothing() == QGL::Smooth ?
                                       "QGL::Smooth" : "QGL::Faceted") << "\n";
    QGL::IndexArray indices = section.indices();
    for (int i = 0; i < section.count(); ++i)
    {
        int ix = indices[i];
        dbg << section.logicalVertexAt(ix) << "\n";
    }
    dbg << ")\n";
    return dbg.space();
}
#endif
