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

#include "spheremesh.h"
#include "qglbuilder.h"
#include "qglsphere.h"
#include "qgraphicsrotation3d.h"
#include "qgraphicsscale3d.h"
#include "qglabstractscene.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass SphereMesh SphereMesh
    \brief The SphereMesh item represents a simple UV-sphere in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The SphereMesh element in QML provides a simple way to create a sphere
    object, usually for testing material effects.  For example,
    the following QML code displays a green sphere mesh of radius 1.5,
    centered on the origin:

    \code
    Item3D {
        mesh: SphereMesh { radius: 1.5 }
        effect: Effect { color: "#aaca00" }
    }
    \endcode

    As shown, the mesh is only displayed when it is set as the target of
    a mesh property on an Item3D.

    The SphereMesh element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    By default, the sphere is aligned to lie along the Z axis.
    The \l axis property can change this to either the X or the Y
    axis, as shown in the following example:

    \code
    SphereMesh {
        radius: 1.5
        axis: Qt.YAxis
    }
    \endcode

    To display the sphere mesh you can create your own Item3D as shown
    above, or use the convenience Sphere QML component:

    \code
    Sphere {
        radius: 1.5
        levelOfDetail: 6
        axis: Qt.YAxis
        effect: Effect { texture: "moon-texture.jpg" }
    }
    \endcode

    \image moon-screenshot.png

    \section1 Performance Hints for Larger Scenes

    The convenience Sphere QML component will create a new mesh each
    time it is instantiated.

    If you have a scene with a number of similar spheres use your own
    Item3D elements to all refer to the same mesh - this will save on
    graphics memory and improve the performance of your application since
    there is no need to recreate many copies of the same geometry.  This
    is true even if you want to apply different materials, effects or
    transformations to each sphere.

    To do this, first declare the SphereMesh object, outside of any Item3D
    so that it won't get drawn, and give it an id so you can refer to it.

    \snippet quick3d/solarsystem_qml/qml/solarsystem_qml.qml 0

    Then set the various effects and so on you need for each instance of
    the sphere.  If you need different sized spheres you can use a scale,
    and of course you can specify materials and effects.

    \snippet quick3d/solarsystem_qml/qml/solarsystem_qml.qml 1

    In this case since only one copy of the triangle data for the mesh is
    sent to the GPU performance will generally be better.

    \section1 Performance Hints for Animation

    Some support for animation of the SphereMesh properties is provided
    by utilizing a QGraphicsScale3D to implement the radius property,
    and by caching levels of detail.

    So within limits animation of these items should provide reasonable
    results.  Be aware that on constrained devices animation of the
    level of detail for many spheres could cause problems with memory
    usage due to the caching.

    The other shapes primitives are implemented differently with respect
    to radius, length and so on, so read the performance notes there, as
    they will differ from the sphere case.

    \sa Item3D
*/

SphereMeshPrivate::SphereMeshPrivate()
    : topNode(new QGLSceneNode)
    , currentSphere(0)
    , rot(0)
    , scale(0)
    , radius(1.0f)
    , lod(5)
    , axis(Qt::ZAxis)
    , sceneSet(false)
{
    topNode->setObjectName("SphereMesh");
}

SphereMeshPrivate::~SphereMeshPrivate()
{
    topNode->removeNode(currentSphere);
    delete topNode;
    QList<QGLSceneNode*> lods = lodGeometry.values();
    qDeleteAll(lods);
}

class SphereScene : public QGLAbstractScene
{
public:
    explicit SphereScene(QGLSceneNode *s) : m_s(s) {}
    ~SphereScene() {}
    QList<QObject *> objects() const
    {
        QList<QObject *> s;
        s.append(m_s);
        return s;
    }
    QGLSceneNode *mainNode() const
    {
        return m_s;
    }
private:
    QGLSceneNode *m_s;
};

/*!
    \internal
*/
SphereMesh::SphereMesh(QObject *parent)
    : QDeclarativeMesh(parent)
    , d_ptr(new SphereMeshPrivate)
{
}

/*!
    \qmlproperty real SphereMesh::radius

    This property defines the radius of the sphere.  The default
    value is 1.
*/
qreal SphereMesh::radius() const
{
    Q_D(const SphereMesh);
    return d->radius;
}

void SphereMesh::setRadius(qreal radius)
{
    Q_D(SphereMesh);
    if (qFuzzyCompare(radius, 1))
        radius = 1.0f;
    if (d->radius != radius)
    {
        d->radius = radius;
        createGeometry();
        emit radiusChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty int SphereMesh::levelOfDetail

    This property defines the level of detail, between 1 and 10,
    which controls the number of triangles that are generated to
    render the surface of the sphere.  Values outside the range
    1 to 10 will be clamped to the range when the sphere is drawn.

    The default value is 5.  An increase in 1 in level of detail
    doubles the number of triangles.  The following picture shows
    the effect of level of detail values between 1 and 10:

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
*/
int SphereMesh::levelOfDetail() const
{
    Q_D(const SphereMesh);
    return d->lod;
}

void SphereMesh::setLevelOfDetail(int lod)
{
    Q_D(SphereMesh);
    lod = qBound(1, lod, 10);
    if (d->lod != lod)
    {
        d->lod = lod;
        createGeometry();
        emit levelOfDetailChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty enumeration SphereMesh::axis

    This property defines the axis that the sphere lies along.
    The default is Qt.ZAxis.  The other supported values are
    Qt.XAxis and Qt.YAxis.  This provides a simple method to
    orient the sphere in the desired direction.
*/
Qt::Axis SphereMesh::axis() const
{
    Q_D(const SphereMesh);
    return d->axis;
}

void SphereMesh::setAxis(Qt::Axis axis)
{
    Q_D(SphereMesh);
    if (d->axis != axis)
    {
        d->axis = axis;
        createGeometry();
        emit axisChanged();
        emit dataChanged();
    }
}

/*!
    \internal
*/
void SphereMesh::draw(QGLPainter *painter, int branchId)
{
    Q_D(SphereMesh);
    if (!d->currentSphere)
        createGeometry();
    QDeclarativeMesh::draw(painter, branchId);
}

/*!
    \internal
*/
void SphereMesh::createGeometry()
{
    Q_D(SphereMesh);

    // We cache a maximum of 10 levels of detail for lod animations.
    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = d->lodGeometry.value(d->lod, 0);
    if (!geometry) {
        QGLBuilder builder;
        builder.newSection(QGL::Faceted);
        builder << QGLSphere(2.0f, d->lod);
        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);
        d->lodGeometry.insert(d->lod, geometry);
    }
    Q_ASSERT_X(geometry != 0, Q_FUNC_INFO, "Could not create/find geometry!");
    if (d->currentSphere != geometry)
    {
        if (d->currentSphere)
            d->topNode->removeNode(d->currentSphere);
        d->topNode->addNode(geometry);
        d->currentSphere = geometry;
    }

    // Set the radius as a scale on the modelview transformation.
    // This way, we don't have to regenerate the geometry every
    // frame if the radius is being animated.
    if (d->radius != 1.0f)
    {
        if (!d->scale)
        {
            d->scale = new QGraphicsScale3D(d->topNode);
            d->topNode->addTransform(d->scale);
        }
        if (d->scale->scale().x() != d->radius)
        {
            d->scale->setScale(QVector3D(d->radius, d->radius, d->radius));
        }
    }
    else
    {
        // If there is already a scale set it to be the identity scale.
        // This case is optimised for in QGraphicsScale.  Removing it from
        // the transform list is too expensive, especially if the size is
        // being animated, and the next frame will recreate it.
        if (d->scale)
            d->scale->setScale(QVector3D(1, 1, 1));
    }

    // Also rotate the geometry into the correct axis orientation.
    const QVector3D Y_AXIS = QVector3D(0, 1, 0);  // for Qt::XAxis we rotate around Y
    const QVector3D X_AXIS = QVector3D(1, 0, 0);  // for Qt::YAxis we rotate around X
    if (d->axis != Qt::ZAxis && !d->rot)
    {
        d->rot = new QGraphicsRotation3D(d->topNode);
        d->topNode->addTransform(d->rot);
    }
    if (d->axis == Qt::XAxis && d->rot->axis().y() != Y_AXIS.y())
    {
        d->rot->setAxis(Y_AXIS);
        d->rot->setAngle(90.0f);
    }
    else if (d->axis == Qt::YAxis && d->rot->axis().x() != X_AXIS.x())
    {
        d->rot->setAxis(X_AXIS);
        d->rot->setAngle(-90.0f);
    }
    else if (d->axis == Qt::ZAxis && d->rot && d->rot->angle() != 0.0f)
    {
        d->rot->setAngle(0.0f);
        d->rot->setAxis(QVector3D(0, 0, 0));
    }

    if (!d->sceneSet)
    {
        setScene(new SphereScene(d->topNode));
        d->sceneSet = true;
    }
}

QT_END_NAMESPACE

