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

#include "qglsphere.h"
#include "qglbuilder.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSphere
    \brief The QGLSphere class represents the geometry of a simple sphere in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a sphere of 2 units in diameter and
    draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder builder;
    builder << QGLSphere(2);
    QGLSceneNode *node = builder.finalizedSceneNode();

    painter.translate(10, 25, 0);
    node->draw(&painter);
    \endcode

    The QGLSphere class specifies positions, normals and 2D texture
    co-ordinates for all of the vertices that make up the sphere.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the sphere can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The default mode of QGLSphere is a "UV sphere", which divides the
    sphere up into longitudinal and latitudinal sections.  The longitudinal
    slices meet at the poles, which in a single unit sphere are defined to
    be at (0, 0, +0.5) and (0, 0, -0.5).  This choice is the simplest to
    texture map as the texture will only distort along the x-axis of the
    2D texture.  However the density of vertices is significantly higher at
    the poles than it is elsewhere on the sphere and is a poor choice if a
    uniform density of pixels from the texture map is required.

    \sa QGLBuilder
*/

/*!
    \fn QGLSphere::QGLSphere(qreal diameter, int depth)

    Creates a sphere of \a diameter across (default is 1).  When the sphere
    is recursively subdivided into triangles, it will be subdivided no more
    than \a depth times (between 1 and 10, default is 5).
*/

/*!
    Destroys this sphere object.
*/
QGLSphere::~QGLSphere()
{
}

/*!
    \fn qreal QGLSphere::diameter() const

    Returns the diameter of this sphere.  The default is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLSphere::setDiameter(qreal diameter)

    Sets the diameter of this sphere to \a diameter.

    \sa diameter()
*/

/*!
    \fn int QGLSphere::subdivisionDepth() const

    Returns the maximum depth when this sphere is subdivided into triangles.
    The default is 5.  The following picture shows the effect of depth
    values between 1 and 10 for a UV sphere:

    \image sphere-detail.png

    \table
    \header \o Level of Detail \o Number of Triangles
    \row \o 1 \o 64
    \row \o 2 \o 128
    \row \o 3 \o 256
    \row \o 4 \o 512
    \row \o 5 \o 1024
    \row \o 6 \o 2048
    \row \o 7 \o 4096
    \row \o 8 \o 8192
    \row \o 9 \o 16384
    \row \o 10 \o 32768
    \endtable

    \sa setSubdivisionDepth()
*/

/*!
    \fn void QGLSphere::setSubdivisionDepth(int depth)

    Sets the maximum \a depth when this sphere is subdivided into triangles.

    \sa subdivisionDepth()
*/

/*!
    \relates QGLSphere

    Builds the geometry for \a sphere within the specified
    geometry \a builder.
*/
QGLBuilder& operator<<(QGLBuilder& builder, const QGLSphere& sphere)
{
    qreal radius = sphere.diameter() / 2.0f;

    // Determine the number of slices and stacks to generate.
    static int const slicesAndStacks[] = {
        8, 4,
        8, 8,
        16, 8,
        16, 16,
        32, 16,
        32, 32,
        64, 32,
        64, 64,
        128, 64,
        128, 128
    };
    int divisions = sphere.subdivisionDepth();
    if (divisions < 1)
        divisions = 1;
    else if (divisions > 10)
        divisions = 10;
    int stacks = slicesAndStacks[divisions * 2 - 1];
    int slices = slicesAndStacks[divisions * 2 - 2];

    // Precompute sin/cos values for the slices and stacks.
    const int maxSlices = 128 + 1;
    const int maxStacks = 128 + 1;
    qreal sliceSin[maxSlices];
    qreal sliceCos[maxSlices];
    qreal stackSin[maxStacks];
    qreal stackCos[maxStacks];
    for (int slice = 0; slice < slices; ++slice) {
        qreal angle = 2 * M_PI * slice / slices;
        sliceSin[slice] = qFastSin(angle);
        sliceCos[slice] = qFastCos(angle);
    }
    sliceSin[slices] = sliceSin[0]; // Join first and last slice.
    sliceCos[slices] = sliceCos[0];
    for (int stack = 0; stack <= stacks; ++stack) {
        qreal angle = M_PI * stack / stacks;
        stackSin[stack] = qFastSin(angle);
        stackCos[stack] = qFastCos(angle);
    }
    stackSin[0] = 0.0f;             // Come to a point at the poles.
    stackSin[stacks] = 0.0f;

    // Create the stacks.
    for (int stack = 0; stack < stacks; ++stack) {
        QGeometryData prim;
        qreal z = radius * stackCos[stack];
        qreal nextz = radius * stackCos[stack + 1];
        qreal s = stackSin[stack];
        qreal nexts = stackSin[stack + 1];
        qreal c = stackCos[stack];
        qreal nextc = stackCos[stack + 1];
        qreal r = radius * s;
        qreal nextr = radius * nexts;
        for (int slice = 0; slice <= slices; ++slice) {
            prim.appendVertex
                (QVector3D(nextr * sliceSin[slice],
                           nextr * sliceCos[slice], nextz));
            prim.appendNormal
                (QVector3D(sliceSin[slice] * nexts,
                           sliceCos[slice] * nexts, nextc));
            prim.appendTexCoord
                (QVector2D(1.0f - qreal(slice) / slices,
                           1.0f - qreal(stack + 1) / stacks));

            prim.appendVertex
                (QVector3D(r * sliceSin[slice],
                           r * sliceCos[slice], z));
            prim.appendNormal
                (QVector3D(sliceSin[slice] * s,
                           sliceCos[slice] * s, c));
            prim.appendTexCoord
                (QVector2D(1.0f - qreal(slice) / slices,
                           1.0f - qreal(stack) / stacks));
        }
        builder.addQuadStrip(prim);
    }

    return builder;
}

QT_END_NAMESPACE
