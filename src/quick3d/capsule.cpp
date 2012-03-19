/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "capsule.h"
#include "qglbuilder.h"
#include "qglcylinder.h"
#include "qgldome.h"

/*!
    \qmlclass Capsule Capsule
    \brief The Capsule item represents a simple capsule in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Capsule element in QML provides a simple way to create a capsule
    object, usually for testing material effects.  For example,
    the following QML code displays a green cylinder of with a uniform
    diameter of 0.5, and a length of 3.0, centered vertically on the
    origin:

    \code
    Capsule {
        radius: 0.5
        length: 3.0
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Capsule element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    The capsule shape can have differing levels of detail, allowing
    for additional slices and layers to be drawn if more detailed meshes
    are required.

    Note that the length of the capsule should always exceed its
    diameter to be considered valid.

    \sa Item3D
*/
/*!
    \internal
*/
Capsule::Capsule(QObject *parent) :
    QQuickItem3D(parent)
    , m_radius(1.0)
    , m_length(4.0)
    , m_lod(1)
{
}

/*!
    \qmlproperty real Capsule::radius

    This property defines the radius of the capsule.
    The default value is 1.
*/
void Capsule::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        emit radiusChanged();
        update();
    }
}

/*!
    \qmlproperty real Capsule::length

    This property defines the length of the capsule.
    The default value is 1.
*/
void Capsule::setLength(qreal length)
{
    if (m_length != length) {
        m_length = length;
        emit lengthChanged();
        update();
    }
}

/*!
    \qmlproperty int Capsule::levelOfDetail

    This property defines the level of detail, between 1 and 10,
    which controls the number of slices and layers that are used
    to generate the capsule's mesh.

    Values outside the range 1 to 10 will be clamped to the range
    when the capsule is generated.

    The exact number of facets generated is consistent with the
    level of detail as specified in the Cylinder and Sphere
    objects.

    \sa Sphere, Cylinder
*/
void Capsule::setLevelOfDetail(int lod)
{
    if (m_lod != lod) {
        m_lod = lod;
        emit levelOfDetailChanged();
        update();
    }
}

/*!
    \internal
*/
void Capsule::drawItem(QGLPainter *painter)
{
    // Convert the level of detail into a depth value for the object.
    // We cache a maximum of 5 levels of detail for lod animations.
    int lod = m_lod;
    if (lod < 1)
        lod = 1;
    else if (lod > 10)
        lod = 10;

    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = m_lodGeometry.value(lod, 0);
    if (!geometry) {
        QGLBuilder builder;

        //For the cylinder
        int facets = 4 * 1<<lod;
        int layers = 1<<lod;

        //For the spheres
        int divisions = lod;

        //sanity check - the height of the capsule must exceed its diameter.  A minimal capsule is
        //a sphere - a capsule where diameter = height.
        if (m_length<2.0*m_radius) {
            qWarning() << "Length of capsule must exceed its diameter - correcting height.";
            m_length = 2*m_radius;
        }

        qreal diameter = m_radius+m_radius;
        qreal cylinderHeight = m_length - diameter;
        qreal offset = cylinderHeight/2.0;

        builder << QGL::Faceted;
        builder.newNode();
        builder << QGLCylinder(diameter, diameter, cylinderHeight, facets, layers, false, false);
        QMatrix4x4 translateMatrix;
        translateMatrix.translate(0, 0, -offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        builder.newNode();
        builder << QGLDome(diameter, divisions, false);
        translateMatrix.setToIdentity();
        translateMatrix.rotate(180, 0, 1,0);
        translateMatrix.translate(0, 0, offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        builder.newNode();
        builder << QGLDome(diameter, divisions, false);
        translateMatrix.setToIdentity();
        translateMatrix.translate(0, 0, offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        geometry = builder.finalizedSceneNode();

        m_lodGeometry.insert(lod, geometry);
    }

    // Draw the geometry.
    geometry->draw(painter);
}
