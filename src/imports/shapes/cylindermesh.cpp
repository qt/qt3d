/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cylindermesh.h"
#include "qglbuilder.h"
#include "qglcylinder.h"
#include "qgraphicsscale3d.h"
#include "qglabstractscene.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass CylinderMesh CylinderMesh
    \brief The CylinderMesh item represents a simple cylinder in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The CylinderMesh element in QML provides a way to create a simple cylinder
    object, usually for testing material effects.  For example, the
    following QML code displays a green cylinder of with a uniform
    diameter of 0.5, and a length of 3.0, centered with its base on the
    origin:

    \code
    Item3D {
        mesh: CylinderMesh {
            radius: 0.5
            length: 3.0
        }
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The CylinderMesh element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    The cylinder shape can have differing levels of detail, allowing
    for additional slices and layers to be drawn if more detailed meshes
    are required.

    To display the cylinder mesh you can create your own Item3D as shown
    above, or use the convenience Cylinder QML Component.

    \code
    Cylinder {
        radius: 0.5
        length: 3.0
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    \section1 Performance Hints for Larger Scenes

    The convenience Cylinder QML component will create a new mesh each
    time is instantiated.

    If you have a scene with a number of similar capsules use your own
    Item3D elements to all refer to the same mesh.  See the Performance
    Hints section of the SphereMesh documentation for more details on
    this important technique.

    \section1 Performance Hints for Animation

    Some support for animation of the CylinderMesh properties is provided
    by utilizing a QGraphicsScale3D to implement the length & radius
    properties, and by caching levels of detail.

    So within limits animation of these items should provide reasonable
    results.  Be aware that on constrained devices animation of the
    level of detail for many cylinders could cause problems with memory
    usage due to the caching.

    \sa Item3D, SphereMesh
*/

CylinderMeshPrivate::CylinderMeshPrivate()
    : topNode(new QGLSceneNode)
    , currentCylinder(0)
    , scale(0)
    , radius(1.0f)
    , length(1.0f)
    , lod(5)
    , sceneSet(false)
{
    topNode->setObjectName("CylinderMesh");
}

CylinderMeshPrivate::~CylinderMeshPrivate()
{
    topNode->removeNode(currentCylinder);
    delete topNode;
    QList<QGLSceneNode*> lods = lodGeometry.values();
    qDeleteAll(lods);
}

class CylinderScene : public QGLAbstractScene
{
public:
    explicit CylinderScene(QGLSceneNode *s) : m_s(s) {}
    ~CylinderScene() {}
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
CylinderMesh::CylinderMesh(QObject *parent)
    : QDeclarativeMesh(parent)
    , d_ptr(new CylinderMeshPrivate)
{
}

/*!
    \qmlproperty real CylinderMesh::radius

    This property defines the radius of the cylinder.
    The default value is 1.
*/
qreal CylinderMesh::radius() const
{
    Q_D(const CylinderMesh);
    return d->radius;
}

void CylinderMesh::setRadius(qreal radius)
{
    Q_D(CylinderMesh);
    if (qFuzzyCompare(radius, 1))
        radius = 1.0f;
    if (d->radius != radius) {
        d->radius = radius;
        createGeometry();
        emit radiusChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty real CylinderMesh::length

    This property defines the length of the cylinder.
    The default value is 1.
*/
qreal CylinderMesh::length() const
{
    Q_D(const CylinderMesh);
    return d->length;
}

void CylinderMesh::setLength(qreal length)
{
    Q_D(CylinderMesh);
    if (qFuzzyCompare(length, 1))
        length = 1.0f;
    if (d->length != length) {
        d->length = length;
        createGeometry();
        emit lengthChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty int CylinderMesh::levelOfDetail

    This property defines the level of detail, between 1 and 10,
    which controls the number of slices and layers that are used
    to generate the cylinder's mesh.

    Values outside the range 1 to 10 will be clamped to the range
    when the cylinder is generated.

    The default value is 1, which produces a cylinder with 8
    slices/facets and 2 layers.  Each successive increase in this
    level of detail doubles the number of facets and layers.  The
    following table provides information for these level of detail
    values

    \table
    \header \o Level of Detail \o Facets \o Layers
    \row \o 1 \o 8 \o 2
    \row \o 2 \o 16 \o 4
    \row \o 3 \o 32 \o 8
    \row \o 4 \o 64 \o 16
    \row \o 5 \o 128 \o 32
    \row \o 6 \o 256 \o 64
    \row \o 7 \o 512 \o 128
    \row \o 8 \o 1024 \o 256
    \row \o 9 \o 2048 \o 512
    \row \o 10 \o 4096 \o 1024
    \endtable
*/
int CylinderMesh::levelOfDetail() const
{
    Q_D(const CylinderMesh);
    return d->lod;
}

void CylinderMesh::setLevelOfDetail(int lod)
{
    Q_D(CylinderMesh);
    lod = qBound(1, lod, 10);
    if (d->lod != lod) {
        d->lod = lod;
        createGeometry();
        emit levelOfDetailChanged();
        emit dataChanged();
    }
}

/*!
    \internal
*/
void CylinderMesh::draw(QGLPainter *painter, int branchId)
{
    Q_D(CylinderMesh);
    if (!d->currentCylinder)
        createGeometry();
    QDeclarativeMesh::draw(painter, branchId);
}

/*!
    \internal
*/
void CylinderMesh::createGeometry()
{
    Q_D(CylinderMesh);

    int facets = 4 * 1 << d->lod;
    int layers = 1 << d->lod;

    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = d->lodGeometry.value(d->lod, 0);
    if (!geometry) {
        QGLBuilder builder;
        builder << QGL::Faceted << QGLCylinder(2.0, 2.0, 1.0, facets,
                                               layers, true, true);
        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);
        d->lodGeometry.insert(d->lod, geometry);
    }
    Q_ASSERT_X(geometry != 0, Q_FUNC_INFO, "Could not create/find geometry!");
    if (d->currentCylinder != geometry)
    {
        if (d->currentCylinder)
            d->topNode->removeNode(d->currentCylinder);
        d->topNode->addNode(geometry);
        d->currentCylinder = geometry;
    }

    // Set the length as a scale on the modelview transformation.
    // This way, we don't have to regenerate the geometry every
    // frame if the height is being animated.
    //
    // We could do the scales separately if either height or radius
    // were != 1.0f, but the same amount of maths is required, so
    // we can just scale appropriately on the axes.
    if (d->radius != 1.0f || d->length != 1.0)
    {
        if (!d->scale)
        {
            d->scale = new QGraphicsScale3D(d->topNode);
            d->topNode->addTransform(d->scale);
        }
        if (d->scale->scale().x() != d->radius ||
                d->scale->scale().z() != d->length)
        {
            d->scale->setScale(QVector3D(d->radius, d->radius, d->length));
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

    if (!d->sceneSet)
    {
        setScene(new CylinderScene(d->topNode));
        d->sceneSet = true;
    }
}

QT_END_NAMESPACE

