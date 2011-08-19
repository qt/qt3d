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

#include "qgldome.h"
#include "qglbuilder.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLDome
    \brief The QGLDome class represents the geometry of a simple hemisphere in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a dome of 2 units in diameter and
    draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder builder;
    builder << QGLDome(2);
    QGLSceneNode *node = builder.finalizedSceneNode();

    // view the dome structure during development
    qDumpScene(node);

    painter.translate(10, 25, 0);
    node->draw(&painter);
    \endcode

    The QGLDome class specifies positions, normals and 2D texture
    co-ordinates for all of the vertices that make up the sphere.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the sphere can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The default mode of QGLDome is half of a "UV sphere", which divides
    the object up into longitudinal and latitudinal sections.  The longitudinal
    slices meet at the pole, which in a single unit dome is defined to
    be at (0, 0, +0.5) and (0, 0, -0.5).  This choice is the simplest to
    texture map as the texture will only distort along the x-axis of the
    2D texture.  However the density of vertices is significantly higher at
    the poles than it is elsewhere on the sphere and is a poor choice if a
    uniform density of pixels from the texture map is required.

    \section1 Dome Sections and Non-Trivial Scene Structure

    When the dome is placed in the builder, by necessity separate sections are created
    so that the correct hard edge for the normals is rendered.  If there were no
    sections, the smooth shading would try to make the dome surface and its flat base
    all part of a smoothly shaded surface instead of having a sharp crease between them.

    Because of this necessary limitation when working with QGLDome always examine the
    generate node structure using the qDumpScene() function, as shown above.  This will
    display the structure of the generated sections by their named scenenodes,and quickly
    make obvious what required code is needed to render the scene as desired.

    In the simplest case disable the base of the dome so that only one simple node is
    generated, and get a handle to this node via builder.currentNode().  Alternatively
    use seperate builder objects and finalizedSceneNode() calls to obtain distinct
    nodes - these will have Base and Dome sibling objects beneath them.

    For other options see \l{QGLCylinder#analysing-and-re-organising-scenes}{Analysing and Re-organising Scenes} in the QGLCylinder class
    documentation.

    Executive summary - use qDumpScene();

    \sa QGLBuilder, QGLCylinder
*/

/*!
    \fn QGLDome::QGLDome(qreal diameter, int depth, bool base)

    Creates a dome of \a diameter across (default is 1).  When the dome
    is recursively subdivided into triangles, it will be subdivided no more
    than \a depth times (between 1 and 5, default is 3).

    If \a base is true, the dome will be drawn with a bottom circle, creating
    an enclosed solid.
*/

/*!
    Destroys this dome object.
*/
QGLDome::~QGLDome()
{
}

/*!
    \fn qreal QGLDome::diameter() const

    Returns the diameter of this dome.  The default is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLDome::setDiameter(qreal diameter)

    Sets the diameter of this dome to \a diameter.

    \sa diameter()
*/

/*!
    \fn int QGLDome::subdivisionDepth() const

    Returns the maximum depth when this hemisphere is subdivided into
    triangles.  The default is 3.  The following picture shows the effect
    of depth values between 1 and 5 for a UV sphere (hemisphere subdivision
    depth shares this scheme).

    \image sphere-detail.png

    \sa setSubdivisionDepth(), QGLSphere::subdivisionDepth()
*/

/*!
    \fn void QGLDome::setSubdivisionDepth(int depth)

    Sets the maximum \a depth when this hemisphere is subdivided into triangles.

    \sa subdivisionDepth()
*/

/*!
    \fn  bool QGLDome::baseEnabled() const

    Returns true if the base of the dome will be created when
    building the mesh.

    The default is true.

    \sa setBaseEnabled()
*/

/*!
    \fn void QGLDome::setBaseEnabled(bool base)

    Set whether the bottom of the dome will be created when
    building the mesh.  If \a base is true, the end-cap will be
    created.

    \sa baseEnabled()
*/

/*!
    \relates QGLDome

    Builds the geometry for \a dome within the specified
    geometry \a builder.
*/
QGLBuilder& operator<<(QGLBuilder& builder, const QGLDome& dome)
{
    qreal radius = dome.diameter() / 2.0f;

    // Determine the number of slices and stacks to generate.
    int divisions = dome.subdivisionDepth();
    if (divisions < 1)
        divisions = 1;
    else if (divisions > 5)
        divisions = 5;
    int stacks = 2 * (1 << divisions);
    int slices = 2 * stacks;
    stacks = stacks>>1;

    // Precompute sin/cos values for the slices and stacks.
    const int maxSlices = 4 * (1 << 5) + 1;
    const int maxStacks = 2 * (1 << 5) + 1;
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

    const qreal halfPi=M_PI/2.0;

    for (int stack = 0; stack <= stacks; ++stack) {
        qreal angle = halfPi * stack / stacks;
        stackSin[stack] = qFastSin(angle);
        stackCos[stack] = qFastCos(angle);
    }
    stackSin[0] = 0.0f;             // Come to a point at the poles.
    stackSin[stacks] = 1.0f;

    builder.newSection();
    builder.currentNode()->setObjectName("Dome");
    // Create the stacks for the dome part of the dome
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
            prim.appendVertex(QVector3D(nextr * sliceSin[slice], nextr * sliceCos[slice], nextz));
            prim.appendNormal(QVector3D(sliceSin[slice] * nexts, sliceCos[slice] * nexts, nextc));
            prim.appendTexCoord(QVector2D(1.0f - qreal(slice) / slices, 1.0f - qreal(stack + 1) / stacks));

            prim.appendVertex(QVector3D(r * sliceSin[slice], r * sliceCos[slice], z));
            prim.appendNormal(QVector3D(sliceSin[slice] * s, sliceCos[slice] * s, c));
            prim.appendTexCoord(QVector2D(1.0f - qreal(slice) / slices, 1.0f - qreal(stack) / stacks));
        }
        builder.addQuadStrip(prim);
    }

    if (dome.baseEnabled()) {
        //Draw end-cap at bottom
        builder.newSection();
        builder.currentNode()->setObjectName("Base");

        //Generate a circle of vertices for this layer.
        QGeometryData tempBase;

        tempBase.appendVertex(QVector3D(0,0,0));
        tempBase.appendTexCoord(QVector2D(0.5,0.5));
        for (int slice=0; slice<=slices+1; slice++)
        {
            tempBase.appendVertex(QVector3D(radius * sliceCos[slice], radius * sliceSin[slice], 0));
            tempBase.appendTexCoord(QVector2D(0.5*sliceCos[slice]+0.5, 0.5*sliceSin[slice]+0.5));
        }

        //we need to reverse the above to draw it properly - windings!
        builder.addTriangulatedFace(tempBase.reversed());
    }
    return builder;
}

QT_END_NAMESPACE

