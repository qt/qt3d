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

#include "qglcylinder.h"
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLCylinder
    \brief The QGLCylinder class represents the geometry of a simple cylinder/cone in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a cone with a top diameter of 1 unit,
    a bottom diameter of  of 2 units in diameter and height of 3 units.

    It then draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder builder;
    builder << QGLCylinder(1.0,2.0,3.0);
    QGLSceneNode *node = builder.finalizedSceneNode();

    // view the cylinder structure during development
    qDumpScene(node);

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

    \section1 Cylinder Sections and Non-Trivial Scene Structure

    When the cylinder is placed in the builder, by necessity separate sections are created
    so that the correct hard edge for the normals is rendered.  If there were no
    sections, the smooth shading would try to make the cylinder sides and its flat bases
    all part of a smoothly shaded surface instead of having a sharp crease between them.

    Because of this necessary limitation when working with QGLCylinder always examine the
    generate node structure using the qDumpScene() function, as shown above.  This will
    display the structure of the generated sections by their named scenenodes,and quickly
    make obvious what required code is needed to render the scene as desired.

    In the case of most non-trivial programatically generate geometry, for example
    compiling cubes, cylinders and domes into a single builder, the resulting scene
    will be not organised in a predictable way.  Due to the section limitation it is hard
    to avoid the results all being siblings.  Using pushNode() does not work because
    section boundaries cause QGLBuilder to reset it's node stack, as the section
    boundaries must be discrete.

    \section1 Analysing and Re-organising Scenes

    To deal with the structure imposed by separate sections, first use qDumpScene() to
    analyse the scene.

    In the simplest case disable the caps of the cylinder so that only one simple node is
    generated, and get a handle to this node via builder.currentNode().  Alternatively
    use seperate builder objects and finalizedSceneNode() calls to obtain distinct
    nodes - these will have Base and Dome sibling objects beneath them.

    In extreme cases the tree can have nodes cut out and transplanted onto alternative
    parents like this:
    \code
    // Here root is the builder's root node, either from the
    // sceneNode() function or from finalizedSceneNode():
    QGLSceneNode *dome = new QGLSceneNode;
    dome->setObjectName("Icecream Top");
    QList<QGLSceneNode*> c = root->allChildren();
    for (int i = 0; i < c.size(); ++i)
    {
        if (c.at(i)->objectName() == "Dome" || c.at(i)->objectName() == "Base")
        {
            root->removeNode(c.at(i));
            dome->addNode(c.at(i));
        }
    }
    dome->setPalette(root->palette());
    dome->setMaterialIndex(domeMat);
    \endcode

    Note that this code relies on knowledge that the nodes "Dome" and "Base" are created
    and this is obtained via qDumpScene.  Using the QGLSceneNode::allChildren() function
    returns a flat list of the heierarchy enabling easy finding of nodes that might be
    deeper in the scene.

    Executive summary - use qDumpScene();

    \sa QGLBuilder
*/


/*!
    \fn QGLCylinder::QGLCylinder(qreal diameterTop, qreal diameterBase , qreal height, int slices, int layers, bool top, bool base)

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
    \fn qreal QGLCylinder::diameterTop() const

    Returns the diameter of the top of the cylinder.

    The default value is 1.

    \sa setDiameterTop()
*/

/*!
    \fn void QGLCylinder::setDiameterTop(qreal diameter)

    Sets the diameter of the top of this cylinder to \a diameter.

    \sa diameterTop()
*/

/*!
    \fn qreal QGLCylinder::diameterBottom() const

    Returns the diameter of the bottom of the cylinder.

    The default value is 1.

    \sa setDiameterBottom()
*/

/*!
    \fn void QGLCylinder::setDiameterBottom(qreal diameter)

    Sets the diameter of the bottom of this cylinder to \a diameter.

    \sa diameterBottom()
*/

/*!
    \fn qreal QGLCylinder::height() const

    Returns the height of the cylinder.

    The default value is 1.0

    \sa setDiameterBottom()
*/

/*!
    \fn void QGLCylinder::setHeight(qreal height)

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
  /*  ASSERT(cylinder.diameterBottom()>=0 &&
           cylinder.diameterTop()>=0 &&
           cylinder.height()>0);*/

    qreal numSlices = qreal(cylinder.slices());
    qreal numLayers = qreal(cylinder.layers());
    qreal topRadius = cylinder.diameterTop()/2.0;
    qreal bottomRadius = cylinder.diameterBottom()/2.0;

    qreal angle = 0;
    qreal angleIncrement = (2.0 * M_PI) / numSlices;
    qreal radius = topRadius;
    qreal radiusIncrement = qreal(bottomRadius-topRadius)/ numLayers;
    qreal height = qreal(cylinder.height());
    qreal heightDecrement = height/numLayers;

    qreal textureHeight = 1.0;
    qreal textureDecrement = 1.0/numLayers;

    QGeometryData oldLayer;

    //Generate vertices for the next layer of cylinder
    for (int layerCount=0; layerCount<=cylinder.layers(); layerCount++) {
        QGeometryData newLayer;

        //Generate a circle of vertices for this layer.
        for (int i=0; i<cylinder.slices(); i++)
        {
            newLayer.appendVertex(QVector3D(radius * qCos(angle),
                                            radius * qSin(angle),
                                            height));
            angle+=angleIncrement;
        }
        angle = 0;
        // Generate texture coordinates (including an extra seam vertex for textures).
        newLayer.appendVertex(newLayer.vertex(0));
        newLayer.generateTextureCoordinates();
        for (int i = 0; i < newLayer.count(); i++)  newLayer.texCoord(i).setY(textureHeight);

        //Special cases for top end-cap
        if (layerCount==0 && cylinder.topEnabled()) {
            //Draw end-cap at top
            QGeometryData top;
            builder.newSection();
            builder.currentNode()->setObjectName("Cylinder Top");
            top.appendVertex(newLayer.center());
            top.appendVertexArray(newLayer.vertices());
            //Generate a circle of texture vertices for this layer.
            top.appendTexCoord(QVector2D(0.5,0.5));

            for (int i=1; i<top.count(); i++)
            {
                top.appendTexCoord(QVector2D(0.5*qCos(angle)+0.5, 0.5*qSin(angle)+0.5));
                angle+=angleIncrement;
            }
            angle = 0;
            builder.addTriangulatedFace(top);
        }


        //Add a new cylinder layer to the mesh
        if (layerCount>0)
        {
            //If it's the first section, create a cylinder sides section
            if (layerCount==1) {
                builder.newSection();
                builder.currentNode()->setObjectName("Cylinder Sides");
            }
            builder.addQuadsInterleaved(oldLayer, newLayer);
        }

        //Special cases for bottom end-cap
        if (layerCount==cylinder.layers()  && cylinder.baseEnabled()) {
            //Draw end-cap at bottom
            QGeometryData base;
            builder.newSection();
            builder.currentNode()->setObjectName("Cylinder Base");
            base.appendVertexArray(newLayer.vertices());
            base.appendVertex(newLayer.center());
            //Generate a circle of texture vertices for this layer.
            for (int i=1; i<base.count(); i++)
            {
                base.appendTexCoord(QVector2D(0.5*qCos(angle)+0.5, 0.5*qSin(angle)+0.5));
                angle+=angleIncrement;
            }
            base.appendTexCoord(QVector2D(0.5,0.5));
            angle = 0;

            //we need to reverse the above to draw it properly - windings!
            builder.addTriangulatedFace(base.reversed());
        }

        //Keep the current layer for drawing the next segment of the cylinder
        oldLayer.clear();
        oldLayer.appendGeometry(newLayer);
        radius+=radiusIncrement;
        height-=heightDecrement;
        textureHeight-=textureDecrement;
    }

    return builder;
}
