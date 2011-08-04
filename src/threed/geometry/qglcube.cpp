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

#include "qglcube.h"
#include "qglbuilder.h"
#include "qvector3darray.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLCube
    \brief The QGLCube class represents the geometry of simple six-sided cube in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example adds a cube of 2 units on a side to a
    geometry builder, and draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder list;
    list.newSection(QGL::Faceted);
    list << QGLCube(2);
    painter->translate(10, 25, 0);
    list.draw(painter);
    \endcode

    QGLCube will create a default set of texture coordinates that shows
    the same texture of all six faces.
*/

/*!
    \fn QGLCube::QGLCube(qreal size)

    Constructs the geometry for a regular cube of \a size
    units on a side.
*/

/*!
    \fn qreal QGLCube::size() const

    Returns the size of this cube.

    \sa setSize()
*/

/*!
    \fn void QGLCube::setSize(qreal size)

    Sets the \a size of this cube.

    \sa size()
*/

static const int vertexDataLen = 6 * 4 * 3;

static const float vertexData[vertexDataLen] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,

    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f
};

static const int texCoordDataLen = 4 * 2;

static const float texCoordData[texCoordDataLen] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};

/*!
    \relates QGLCube

    Builds the geometry for \a cube within the specified
    geometry \a builder.

    This operator specifies the positions, and 2D texture
    co-ordinates for all of the vertices that make up the cube.
    Normals will be calculated by the \a builder, depending on its
    current section's smoothing setting.
*/
QGLBuilder& operator<<(QGLBuilder& builder, const QGLCube& cube)
{
    QGeometryData op;

    QVector3DArray vrts = QVector3DArray::fromRawData(
            reinterpret_cast<const QVector3D *>(vertexData), vertexDataLen / 3);
    if (cube.size() != 1.0f)
        vrts.scale(cube.size());

    op.appendVertexArray(vrts);

    QVector2DArray texx = QVector2DArray::fromRawData(
            reinterpret_cast<const QVector2D *>(texCoordData), texCoordDataLen / 2);

    for (int i = 0; i < 6; ++i)
        op.appendTexCoordArray(texx);

    builder.addQuads(op);
    return builder;
}

QT_END_NAMESPACE
