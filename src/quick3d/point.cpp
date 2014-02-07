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

#include "point.h"
#include "qglbuilder.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype Point
    \instantiates Point
    \brief The Point item represents a geometric point drawn in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Point element in QML provides a means of drawing points in a 3D
    environment.  For example, the following QML code draws points (0,0,0),
    (1,1,1), and (-1,-1,-1).

    \code
    Point {
        vertices: [
           0, 0, 0,
           1, 1, 1,
           -1, -1, -1
        ]
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Point element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 2.0
    \endcode

    \sa Item3D
*/
/*!
    \internal
*/
Point::Point(QObject *parent) :
    QQuickItem3D(parent)
    , m_pointSize(1.0f)
    , m_geometry(0)
    , m_changeFlag(false)
{
    //meh
}

/*!
    \internal
*/
Point::~Point()
{
    delete m_geometry;
}


/*!
   \qmlproperty list<real> Point::vertices

   This property defines the positions for all of the points to be drawn
   Each vertex is given by three real values, defining
   the x, y, and z co-ordinates of the vertex.
*/

QVariant Point::vertices() const
{
   return m_vertices;
}

void Point::setVertices(const QVariant &value)
{
    m_vertices = value;

    //Update the actual QVector3DArray containing the points.
    m_vertexArray.clear();
    QVariantList vertlist = m_vertices.toList();
    for (int index = 0; (index + 2) < vertlist.size(); index += 3) {
        m_vertexArray.append(vertlist.at(index).toFloat(), vertlist.at(index + 1).toFloat(), vertlist.at(index + 2).toFloat());
    }
    m_changeFlag=true;
    emit verticesChanged();
    update();
}


/*!
    \qmlproperty float Point::pointSize

    This property defines the size of the point.  The
    default is 1.0
*/
void Point::setPointSize(float pointSize)
{
    if (m_pointSize != pointSize) {
        m_pointSize = pointSize;
        m_changeFlag=true;
        emit pointSizeChanged();
        update();
    }
}

/*!
    \internal
*/
void Point::drawItem(QGLPainter *painter)
{
    if (m_changeFlag || !m_geometry) {
        if (m_geometry) delete m_geometry;

        QGLBuilder builder;

        QGeometryData pointCollection;
        builder.newSection();

        pointCollection.appendVertexArray(m_vertexArray);

        builder.addTriangles(pointCollection);
        builder.currentNode()->setDrawingMode(QGL::Points);
        builder.currentNode()->setDrawingWidth(m_pointSize);
        m_geometry = builder.finalizedSceneNode();

        m_changeFlag = false;
    }

    // Draw the geometry.
    m_geometry->draw(painter);
}

QT_END_NAMESPACE


