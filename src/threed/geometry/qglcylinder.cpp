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

#include "qglcylinder.h"
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtCore/qmath.h>

#include <cmath>

QT_BEGIN_NAMESPACE

/*!
    \class QGLCylinder
    \brief The QGLCylinder class represents the geometry of a simple cylinder/cone in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a cone with a top diameter of 1 unit,
    a bottom diameter of 2 units in diameter and height of 3 units.

    It then draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder builder;
    builder << QGLCylinder(1.0,2.0,3.0);
    QGLSceneNode *node = builder.finalizedSceneNode();

    painter.translate(10, 25, 0);
    node->draw(&painter);
    \endcode

    Note that the bottom circle of the cylinder will always be centred at (0,0,0)
    unless otherwise transformed after cylinder creation.

    The QGLCylinder class specifies positions, normals and 2D texture
    co-ordinates for all of the vertices that make up the cylinder.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the cylinder can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The QGLCylinder is divided into slices and layers.  The slices value
    indicate number of triangular sections into which the top and bottom
    circles of the cylinder are broken into.  Consequently it also sets the
    number of facets which run the length of the cylinder.  More slices
    results in a smoother circumference.

    The layers value indicates the number of longitudinal sections the
    cylinder is broken into.  Fewer layers means that the side facets of the
    cylinder will be made up of fewer, very long, triangles, while a higher
    number of layers will produce many and smaller triangles.  Often it is
    desirable to avoid large triangles as they may cause inefficiencies in
    texturing/lighting on certain platforms.

    The end-caps and sides of the cylinder are independent sections of the
    scene-graph, and so may be textured separately.

    Textures are wrapped around the sides of thecylinder in such a way that
    the texture may distort across the x axis if the top and bottom diameters
    of the cylinder differ (ie. the cylinder forms a truncated cone).  Textures
    begin and end at the centre points of the top and bottom end-caps of the
    cylinder.  This wrapping means that textures on either end-cap may be
    distorted.

    Texture coordinates are assigned as shown below.

    \image cylinder-texture-coords.png

    It is worth noting that the cylinder class can, in fact, be used to generate
    any regular solid polygonal prism.  A rectangular prism can be created, for
    example, by creating a 4 sided cylinder.  Likewise a hexagonal prism is
    simply a 6 sided cylinder.

    With this knowledge, and an understanding of the texture coordinate mapping,
    it is possible to make custom textures which will be usable with these
    three dimensional objects.

    \sa QGLBuilder
*/


/*!
    \fn QGLCylinder::QGLCylinder(float diameterTop, float diameterBase , float height, int slices, int layers, bool top, bool base)

    Constructs the geometry for a cylinder with top of diameter \a diameterTop,
    a base of diameter \a diameterBase, and a height of \a height.

    The resultant mesh will be divided around the vertical axis of the cylinder
    into \a slices individual wedges, and shall be formed of \a layers stacked
    to form the cylinder.

    If the values for \a top or \a base are true, then the cylinder will be
    created with solid endcaps.  Otherwise, it shall form a hollow pipe.

    units on a side.
*/


/*!
    \fn float QGLCylinder::diameterTop() const

    Returns the diameter of the top of the cylinder.

    The default value is 1.

    \sa setDiameterTop()
*/

/*!
    \fn void QGLCylinder::setDiameterTop(float diameter)

    Sets the diameter of the top of this cylinder to \a diameter.

    \sa diameterTop()
*/

/*!
    \fn float QGLCylinder::diameterBottom() const

    Returns the diameter of the bottom of the cylinder.

    The default value is 1.

    \sa setDiameterBottom()
*/

/*!
    \fn void QGLCylinder::setDiameterBottom(float diameter)

    Sets the diameter of the bottom of this cylinder to \a diameter.

    \sa diameterBottom()
*/

/*!
    \fn float QGLCylinder::height() const

    Returns the height of the cylinder.

    The default value is 1.0

    \sa setDiameterBottom()
*/

/*!
    \fn void QGLCylinder::setHeight(float height)

    Sets the height of this cylinder to \a height.

    \sa diameterBottom()
*/


/*!
    \fn int QGLCylinder::slices() const

    Returns the number of triangular slices the cylinder is divided into
    around its polar axis.

    The default is 6.

    \sa setSlices()
*/

/*!
    \fn int QGLCylinder::setSlices(int slices)

    Sets the number of triangular \a slices the cylinder is divided into
    around its polar axis.

    \sa slices()
*/

/*!
    \fn int QGLCylinder::layers() const

    Returns the number of cylindrical layers the cylinder is divided into
    along its height.

    The default is 3.

    \sa setLayers()
*/

/*!
    \fn int QGLCylinder::setLayers(int layers)

    Sets the number of stacked \a layers the cylinder is divided into
    along its height.

    \sa layers()
*/

/*!
    \fn  bool QGLCylinder::topEnabled() const

    Returns true if the top of the cyclinder will be created when
    building the mesh.

    The default is true.

    \sa setTopEnabled()
*/

/*!
    \fn void QGLCylinder::setTopEnabled(bool top)

    Set whether the top end-cap of the cylinder will be created when
    building the mesh.  If \a top is true, the end-cap will be created.

    \sa topEnabled()
*/

/*!
    \fn  bool QGLCylinder::baseEnabled() const

    Returns true if the base of the cyclinder will be created when
    building the mesh.

    The default is true.

    \sa setBaseEnabled()
*/

/*!
    \fn void QGLCylinder::setBaseEnabled(bool base)

    Set whether the base end-cap of the cylinder will be created when
    building the mesh.  If \a base is true, the end-cap will be created.

    \sa baseEnabled()
*/

/*!
    \relates QGLCylinder

    Builds the geometry for \a cylinder within the specified
    geometry \a builder.
*/

QGLBuilder& operator<<(QGLBuilder& builder, const QGLCylinder& cylinder)
{
    int nCaps = (cylinder.topEnabled()?1:0) + (cylinder.baseEnabled()?1:0);
    Q_ASSERT(cylinder.layers() >= 1 + nCaps);

    float numSlices = float(cylinder.slices());
    float numLayers = float(cylinder.layers() - nCaps); // minus top and base caps
    float topRadius = cylinder.diameterTop() / 2.0f;
    float bottomRadius = cylinder.diameterBottom() / 2.0f;

    float angle = 0.0f;
    float angleIncrement = (2.0f * M_PI) / numSlices;
    float radius = topRadius;
    float radiusIncrement = float(bottomRadius-topRadius) / numLayers;
    float height = float(cylinder.height());
    float heightDecrement = height / numLayers;
    height *= 0.5f;

    float textureHeight = 1.0f;
    float textureDecrement = 1.0f / numLayers;

    QGeometryData oldLayer;

    // layer 0: Top cap
    {
        QGeometryData newLayer;
        //Generate a circle of vertices for this layer.
        for (int i=0; i<cylinder.slices(); i++) {
            newLayer.appendVertex(QVector3D(radius * cosf(angle), radius * sinf(angle), height));
            angle+=angleIncrement;
        }
        angle = 0.0f;
        QVector3D center = newLayer.center();
        // Generate texture coordinates (including an extra seam vertex for textures).
        newLayer.appendVertex(newLayer.vertex(0));
        newLayer.generateTextureCoordinates();
        for (int i = 0; i < newLayer.count(); ++i)
            newLayer.texCoord(i).setY(textureHeight);
        if (cylinder.topEnabled()) {
            QGeometryData top;
            builder.newSection();
            builder.currentNode()->setObjectName(QStringLiteral("Cylinder Top"));
            top.appendVertex(center);
            top.appendVertexArray(newLayer.vertices());
            //Generate a circle of texture vertices for this layer.
            top.appendTexCoord(QVector2D(0.5f, 0.5f));
            for (int i=1; i<top.count(); i++) {
                top.appendTexCoord(QVector2D(0.5f * cosf(angle) + 0.5f, 0.5f * sinf(angle) + 0.5f));
                angle+=angleIncrement;
            }
            angle = 0;
            builder.addTriangulatedFace(top);
        }
        oldLayer.clear();
        oldLayer.appendGeometry(newLayer);
    }

    // intermediate layers
    for (int layerCount=0; layerCount<(cylinder.layers()-nCaps); ++layerCount) {
        radius+=radiusIncrement;
        height-=heightDecrement;
        textureHeight-=textureDecrement;
        QGeometryData newLayer;
        //Generate a circle of vertices for this layer.
        for (int i=0; i<cylinder.slices(); ++i) {
            newLayer.appendVertex(QVector3D(radius * cosf(angle), radius * sinf(angle), height));
            angle+=angleIncrement;
        }
        angle = 0.0f;
        // Generate texture coordinates (including an extra seam vertex for textures).
        newLayer.appendVertex(newLayer.vertex(0));
        newLayer.generateTextureCoordinates();
        for (int i = 0; i < newLayer.count(); ++i)
            newLayer.texCoord(i).setY(textureHeight);

        if (layerCount==0) {
            builder.newSection();
            builder.currentNode()->setObjectName(QStringLiteral("Cylinder Sides"));
        }
        builder.addQuadsInterleaved(oldLayer, newLayer);

        oldLayer.clear();
        oldLayer.appendGeometry(newLayer);
    }

    // last layer: Base cap
    if (cylinder.baseEnabled()) {
        builder.newSection();
        builder.currentNode()->setObjectName(QStringLiteral("Cylinder Base"));
        QGeometryData base;
        {
            QVector3DArray vvv = oldLayer.vertices();
            for (int i=0; i<vvv.size()-1; ++i)
                base.appendVertex(vvv.at(i));
            QVector3D center = base.center();
            base.appendVertex(vvv.at(0));
            base.appendVertex(center);
        }
        //Generate a circle of texture vertices for this layer.
        for (int i=1; i<base.count(); i++) {
            base.appendTexCoord(QVector2D(0.5f * cosf(angle) + 0.5f, 0.5f * sinf(angle) + 0.5f));
            angle+=angleIncrement;
        }
        base.appendTexCoord(QVector2D(0.5f, 0.5f));
        angle = 0.0f;
        //we need to reverse the above to draw it properly - windings!
        builder.addTriangulatedFace(base.reversed());
    }

    return builder;
}

QT_END_NAMESPACE
