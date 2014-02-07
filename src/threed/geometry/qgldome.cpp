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

    \sa QGLBuilder
*/

/*!
    \fn QGLDome::QGLDome(float diameter, int depth, bool base)

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
    \fn float QGLDome::diameter() const

    Returns the diameter of this dome.  The default is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLDome::setDiameter(float diameter)

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
    float radius = dome.diameter() / 2.0f;

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
    float sliceSin[maxSlices];
    float sliceCos[maxSlices];
    float stackSin[maxStacks];
    float stackCos[maxStacks];
    for (int slice = 0; slice < slices; ++slice) {
        float angle = 2.0f * M_PI * slice / slices;
        sliceSin[slice] = qFastSin(angle);
        sliceCos[slice] = qFastCos(angle);
    }
    sliceSin[slices] = sliceSin[0]; // Join first and last slice.
    sliceCos[slices] = sliceCos[0];

    const float halfPi = M_PI / 2.0;

    for (int stack = 0; stack <= stacks; ++stack) {
        float angle = halfPi * stack / stacks;
        stackSin[stack] = qFastSin(angle);
        stackCos[stack] = qFastCos(angle);
    }
    stackSin[0] = 0.0f;             // Come to a point at the poles.
    stackSin[stacks] = 1.0f;

    builder.newSection();
    builder.currentNode()->setObjectName(QStringLiteral("Dome"));
    // Create the stacks for the dome part of the dome
    for (int stack = 0; stack < stacks; ++stack) {
        QGeometryData prim;
        float z = radius * stackCos[stack];
        float nextz = radius * stackCos[stack + 1];
        float s = stackSin[stack];
        float nexts = stackSin[stack + 1];
        float c = stackCos[stack];
        float nextc = stackCos[stack + 1];
        float r = radius * s;
        float nextr = radius * nexts;
        for (int slice = 0; slice <= slices; ++slice) {
            prim.appendVertex(QVector3D(nextr * sliceSin[slice], nextr * sliceCos[slice], nextz));
            prim.appendNormal(QVector3D(sliceSin[slice] * nexts, sliceCos[slice] * nexts, nextc));
            prim.appendTexCoord(QVector2D(1.0f - float(slice) / slices, 1.0f - float(stack + 1) / stacks));

            prim.appendVertex(QVector3D(r * sliceSin[slice], r * sliceCos[slice], z));
            prim.appendNormal(QVector3D(sliceSin[slice] * s, sliceCos[slice] * s, c));
            prim.appendTexCoord(QVector2D(1.0f - float(slice) / slices, 1.0f - float(stack) / stacks));
        }
        builder.addQuadStrip(prim);
    }

    if (dome.baseEnabled()) {
        //Draw end-cap at bottom
        builder.newSection();
        builder.currentNode()->setObjectName(QStringLiteral("Base"));

        //Generate a circle of vertices for this layer.
        QGeometryData tempBase;

        tempBase.appendVertex(QVector3D(0.0f, 0.0f, 0.0f));
        tempBase.appendTexCoord(QVector2D(0.5f, 0.5f));
        for (int slice=0; slice<=slices+1; slice++)
        {
            tempBase.appendVertex(QVector3D(radius * sliceCos[slice], radius * sliceSin[slice], 0.0f));
            tempBase.appendTexCoord(QVector2D(0.5f * sliceCos[slice] + 0.5f, 0.5f * sliceSin[slice] + 0.5f));
        }

        //we need to reverse the above to draw it properly - windings!
        builder.addTriangulatedFace(tempBase.reversed());
    }
    return builder;
}

QT_END_NAMESPACE

