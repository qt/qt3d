/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "sphere.h"
#include "qglbuilder.h"
#include "qglsphere.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Sphere Sphere
    \brief The Sphere item represents a simple UV-sphere in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Sphere element in QML provides a simple way to create a sphere
    object, usually for testing material effects.  For example,
    the following QML code displays a green sphere of radius 1.5,
    centered on the origin:

    \code
    Sphere {
        radius: 1.5
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Sphere element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    By default, the sphere is aligned to lie along the Z axis.
    The \l axis property can change this to either the X or the Y
    axis, as shown in the following example:

    \code
    Sphere {
        scale: 1.5
        levelOfDetail: 6
        axis: Qt.YAxis
        effect: Effect { texture: "moon-texture.jpg" }
    }
    \endcode

    \image moon-screenshot.png

    \sa Item3D
*/
/*!
    \internal
*/
Sphere::Sphere(QObject *parent)
    : QQuickItem3D(parent)
    , m_radius(1.0f)
    , m_lod(-1)
    , m_axis(Qt::ZAxis)
{
    setLevelOfDetail(5);
}

/*!
    \qmlproperty real Sphere::radius

    This property defines the radius of the sphere.  The default
    value is 1.
*/
void Sphere::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        emit radiusChanged();
        update();
    }
}

/*!
    \qmlproperty int Sphere::levelOfDetail

    This property defines the level of detail, between 1 and 10,
    which controls the number of triangles that are generated to
    render the surface of the sphere.  Values outside the range
    1 to 10 will be clamped.

    The default value is 5.  An increase in 1 in level of detail
    doubles the number of triangles.  The following picture shows
    the effect of level of detail values between 1 and 10:

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
*/
void Sphere::setLevelOfDetail(int lod)
{
    if (lod < 1)
    {
        lod = 1;
    }
    else
    {
        if (lod > 10)
            lod = 10;
    }

    if (m_lod != lod) {
        m_lod = lod;
        updateGeometry();
        emit levelOfDetailChanged();
        update();
    }
}

/*!
    \qmlproperty enumeration Sphere::axis

    This property defines the axis that the sphere lies along.
    The default is Qt.ZAxis.  The other supported values are
    Qt.XAxis and Qt.YAxis.  This provides a simple method to
    orient the sphere in the desired direction.
*/
void Sphere::setAxis(Qt::Axis axis)
{
    if (m_axis != axis) {
        m_axis = axis;
        emit axisChanged();
        update();
    }
}

/*!
    \internal
*/
void Sphere::updateGeometry()
{
    // Convert the level of detail into a depth value for QGLSphere.
    // We cache a maximum of 10 levels of detail for lod animations.

    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = m_lodGeometry.value(m_lod, 0);
    if (!geometry) {
        QGLBuilder builder;
        builder.newSection(QGL::Faceted);
        builder << QGLSphere(2.0f, m_lod);
        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);
        m_lodGeometry.insert(m_lod, geometry);
    }
}

void Sphere::drawItem(QGLPainter *painter)
{
    // Set the radius as a scale on the modelview transformation.
    // This way, we don't have to regenerate the geometry every
    // frame if the radius is being animated.  Also rotate the
    // geometry into the correct axis orientation.
    if (m_radius != 1.0f || m_axis != Qt::ZAxis) {
        painter->modelViewMatrix().push();
        if (m_radius != 1.0f)
            painter->modelViewMatrix().scale(m_radius);
        if (m_axis == Qt::XAxis)
            painter->modelViewMatrix().rotate(90.0f, 0.0f, 1.0f, 0.0);
        else if (m_axis == Qt::YAxis)
            painter->modelViewMatrix().rotate(-90.0f, 1.0f, 0.0f, 0.0);
    }

    // Draw the geometry.
    QGLSceneNode *geometry = m_lodGeometry.value(m_lod, 0);
    geometry->draw(painter);

    // Restore the original modelview.
    if (m_radius != 1.0f || m_axis != Qt::ZAxis)
        painter->modelViewMatrix().pop();
}

QT_END_NAMESPACE
