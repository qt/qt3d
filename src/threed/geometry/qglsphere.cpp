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
    \fn QGLSphere::QGLSphere(float diameter, int depth)

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
    \fn float QGLSphere::diameter() const

    Returns the diameter of this sphere.  The default is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLSphere::setDiameter(float diameter)

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
    \header \li Level of Detail \li Number of Triangles
    \row \li 1 \li 64
    \row \li 2 \li 128
    \row \li 3 \li 256
    \row \li 4 \li 512
    \row \li 5 \li 1024
    \row \li 6 \li 2048
    \row \li 7 \li 4096
    \row \li 8 \li 8192
    \row \li 9 \li 16384
    \row \li 10 \li 32768
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
    float radius = sphere.diameter() / 2.0f;

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
    float sliceSin[maxSlices];
    float sliceCos[maxSlices];
    float stackSin[maxStacks];
    float stackCos[maxStacks];
    for (int slice = 0; slice < slices; ++slice) {
        float angle = 2.0f * M_PI * (slices - 1 - slice) / slices;
        sliceSin[slice] = qFastSin(angle);
        sliceCos[slice] = qFastCos(angle);
    }
    sliceSin[slices] = sliceSin[0]; // Join first and last slice.
    sliceCos[slices] = sliceCos[0];
    for (int stack = 0; stack <= stacks; ++stack) {
        float angle = M_PI * stack / stacks;
        stackSin[stack] = qFastSin(angle);
        stackCos[stack] = qFastCos(angle);
    }
    stackSin[0] = 0.0f;             // Come to a point at the poles.
    stackSin[stacks] = 0.0f;

    // Create the stacks.
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
            prim.appendVertex
                (QVector3D(nextr * sliceSin[slice],
                           nextr * sliceCos[slice], nextz));
            prim.appendNormal
                (QVector3D(sliceSin[slice] * nexts,
                           sliceCos[slice] * nexts, nextc));
            prim.appendTexCoord
                (QVector2D(1.0f - float(slice) / slices,
                           1.0f - float(stack + 1) / stacks));

            prim.appendVertex
                (QVector3D(r * sliceSin[slice],
                           r * sliceCos[slice], z));
            prim.appendNormal
                (QVector3D(sliceSin[slice] * s,
                           sliceCos[slice] * s, c));
            prim.appendTexCoord
                (QVector2D(1.0f - float(slice) / slices,
                           1.0f - float(stack) / stacks));
        }
        builder.addQuadStrip(prim);
    }

    return builder;
}

QT_END_NAMESPACE
