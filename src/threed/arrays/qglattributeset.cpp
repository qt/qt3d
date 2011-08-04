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

#include "qglattributeset.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLAttributeSet
    \brief The QGLAttributeSet class provides a set of QGL::VertexAttribute indexes.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::arrays

    QGLAttributeSet is intended for checking if a specific vertex
    attribute is present in a piece of geometry, or has been set on a
    QGLPainter during rendering operations.  The members of the set
    are instances of QGL::VertexAttribute, with the restriction that
    the index must be between 0 and 31.

    The most common use for this class is to determine if specific
    attributes have been supplied on a QGLPainter so as to adjust the
    current drawing effect accordingly.  The following example will
    use a lit texture effect if texture co-ordinates were provided
    in the vertex bundle, or a simple lit material effect if
    texture co-ordinates were not provided:

    \code
    painter.clearAttributes();
    painter.setVertexBundle(bundle);
    if (painter.attributes().contains(QGL::TextureCoord0))
        painter.setStandardEffect(QGL::LitModulateTexture2D);
    else
        painter.setStandardEffect(QGL::LitMaterial);
    \endcode

    It is important to clear the attributes before setting the vertex
    bundle, so that attributes from a previous bundle will not leak
    through.  Multiple vertex bundles may be supplied if they contain
    different parts of the same logical piece of geometry.

    \sa QGLVertexBundle::attributes(), QGLPainter::attributes()
*/

/*!
    \fn QGLAttributeSet::QGLAttributeSet()

    Constructs an empty attribute set.

    \sa isEmpty()
*/

/*!
    \fn bool QGLAttributeSet::isEmpty() const

    Returns true if this attribute set is empty; false otherwise.
*/

/*!
    \fn void QGLAttributeSet::clear()

    Clears this attribute set to empty.
*/

/*!
    \fn bool QGLAttributeSet::contains(QGL::VertexAttribute attr) const

    Returns true if this attribute set contains \a attr; false otherwise.

    \sa insert(), remove()
*/

/*!
    \fn void QGLAttributeSet::insert(QGL::VertexAttribute attr)

    Inserts \a attr into this attribute set.  Note: \a attr must be
    within the range 0 to 31.  Attribute indexes outside this range
    are ignored and not added to the set.

    \sa remove(), contains()
*/

/*!
    \fn void QGLAttributeSet::remove(QGL::VertexAttribute attr)

    Removes \a attr from this attribute set.

    \sa insert(), contains()
*/

/*!
    Returns the members of this attribute set as a list.

    \sa fromList()
*/
QList<QGL::VertexAttribute> QGLAttributeSet::toList() const
{
    QList<QGL::VertexAttribute> list;
    quint32 attrs = m_attrs;
    int index = 0;
    while (attrs != 0) {
        if ((attrs & 1) != 0)
            list.append(QGL::VertexAttribute(index));
        ++index;
        attrs >>= 1;
    }
    return list;
}

/*!
    Returns a new attribute set that is initialized with the members
    of \a list.

    \sa toList(), insert()
*/
QGLAttributeSet QGLAttributeSet::fromList(const QList<QGL::VertexAttribute> &list)
{
    QGLAttributeSet set;
    for (int index = 0; index < list.size(); ++index)
        set.insert(list.at(index));
    return set;
}

/*!
    \fn void QGLAttributeSet::unite(const QGLAttributeSet &other)

    Unites the contents of \a other with this attribute set
    and modifies this set accordingly.

    \sa intersect(), subtract(), insert()
*/

/*!
    \fn void QGLAttributeSet::intersect(const QGLAttributeSet &other)

    Intersects the contents of \a other with this attribute set
    and modifies this set accordingly.

    \sa unite(), subtract()
*/

/*!
    \fn void QGLAttributeSet::subtract(const QGLAttributeSet &other)

    Subtracts the contents of \a other from this attribute set
    and modifies this set accordingly.

    \sa unite(), intersect(), remove()
*/

/*!
    \fn bool QGLAttributeSet::operator==(const QGLAttributeSet &other) const

    Returns true if this attribute set has the same elements as \a other;
    false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QGLAttributeSet::operator!=(const QGLAttributeSet &other) const

    Returns true if this attribute set does not have the same elements as
    \a other; false otherwise.

    \sa operator==()
*/

QT_END_NAMESPACE
