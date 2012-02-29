/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "capsulemesh.h"
#include "qglbuilder.h"
#include "qglcylinder.h"
#include "qgldome.h"
#include "qglabstractscene.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass CapsuleMesh CapsuleMesh
    \brief The CapsuleMesh item represents a simple capsule in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The CapsuleMesh element in QML provides a simple way to create a capsule
    object, usually for testing material effects.  For example,
    the following QML code displays a green capsule of with a uniform
    diameter of 0.5, and a length of 3.0, centered vertically on the
    origin:

    \code
    Item3D {
        mesh: CapsuleMesh {
            radius: 0.5
            length: 3.0
        }
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The CapsuleMesh element is part of the \c{Qt3D.Shapes} namespace,
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

    To display the capsule mesh you can create your own Item3D as shown
    above, or use the convenience Capsule QML Component.

    \code
    Capsule {
        radius: 0.5
        length: 3.0
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    \section1 Performance Hints for Larger Scenes

    The convenience Capsule QML component will create a new mesh each
    time is instantiated.

    If you have a scene with a number of similar capsules use your own
    Item3D elements to all refer to the same mesh.  See the Performance
    Hints section of the SphereMesh documentation for more details on
    this important technique.

    \section1 Performance Hints for Animation

    Because of the need to maintain the relative proportions of the
    cylinders components, the length and radius are implemented directly
    in the construction of the mesh.  This differs from the approach
    used in the SphereMesh where a scale is used.

    For this reason avoid animating the length and radius properties
    of a CapsuleMesh, as it will give poor results due to the mesh
    being recreated every frame.

    Since the capules components are created with named sections in
    the mesh, one approach would be to individually apply scales to
    these sections using the meshNode property.  This is an
    advanced topic, and future tutorials may cover it in more depth.

    \sa Item3D, SphereMesh
*/

class CapsuleMeshPrivate
{
public:
    CapsuleMeshPrivate();
    ~CapsuleMeshPrivate();

    QMap<int, QGLSceneNode *> lodGeometry;
    QGLSceneNode *topNode;
    QGLSceneNode *currentCapsule;
    qreal radius;
    qreal length;
    int lod;
    bool sceneSet;
};

CapsuleMeshPrivate::CapsuleMeshPrivate()
    : topNode(new QGLSceneNode)
    , currentCapsule(0)
    , radius(1.0f)
    , length(2.0f)
    , lod(5)
    , sceneSet(false)
{
    topNode->setObjectName("CapsuleMesh");
}

CapsuleMeshPrivate::~CapsuleMeshPrivate()
{
    topNode->removeNode(currentCapsule);
    delete topNode;
    QList<QGLSceneNode*> lods = lodGeometry.values();
    qDeleteAll(lods);
}

class CapsuleScene : public QGLAbstractScene
{
public:
    explicit CapsuleScene(QGLSceneNode *s) : m_s(s) {}
    ~CapsuleScene() {}
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
CapsuleMesh::CapsuleMesh(QObject *parent)
    : QDeclarativeMesh(parent)
    , d(new CapsuleMeshPrivate)
{
}

/*!
    \internal
*/
CapsuleMesh::~CapsuleMesh()
{
    delete d;
}

/*!
    \qmlproperty real CapsuleMesh::radius

    This property defines the radius of the capsule.
    The default value is 1.
*/
qreal CapsuleMesh::radius() const
{
    return d->radius;
}

void CapsuleMesh::setRadius(qreal radius)
{
    if (qFuzzyCompare(radius, 1))
        radius = 1.0f;
    if (d->radius != radius) {
        d->radius = radius;
        createGeometry(true);
        emit radiusChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty real CapsuleMesh::length

    This property defines the length of the capsule.
    The default value is 1.
*/
qreal CapsuleMesh::length() const
{
    return d->length;
}

void CapsuleMesh::setLength(qreal length)
{
    if (qFuzzyCompare(length, 1))
        length = 1.0f;
    if (d->length != length) {
        d->length = length;
        createGeometry(true);
        emit lengthChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty int CapsuleMesh::levelOfDetail

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
int CapsuleMesh::levelOfDetail() const
{
    return d->lod;
}

void CapsuleMesh::setLevelOfDetail(int lod)
{
    lod = qBound(1, lod, 10);
    if (d->lod != lod) {
        d->lod = lod;
        createGeometry(true);
        emit levelOfDetailChanged();
        emit dataChanged();
    }
}

/*!
    \internal
*/
void CapsuleMesh::draw(QGLPainter *painter, int branchId)
{
    if (!d->currentCapsule)
        createGeometry();
    QDeclarativeMesh::draw(painter, branchId);
}

/*!
    \internal
*/
void CapsuleMesh::createGeometry(bool bForce)
{
    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = 0;
    QMap<int, QGLSceneNode *>::iterator It = d->lodGeometry.find(d->lod);
    if (It != d->lodGeometry.end())
        geometry = *It;
    if (geometry && bForce) {
        if (d->currentCapsule)
            d->topNode->removeNode(d->currentCapsule);
        d->currentCapsule = 0;
        d->lodGeometry.erase(It);
        geometry->setParent(0);
        delete geometry;
        geometry = 0;
    }
    if (!geometry) {
        QGLBuilder builder;

        // For the cylinder
        int facets = 4 * (1 << d->lod);
        int layers =     (1 << d->lod) - 1;

        // For the spheres
        int divisions = d->lod;

        // Sanity check - the height of the capsule must not be less than its
        // diameter.  A minimal capsule is a sphere - where diameter == height.
        if (d->length < 2.0 * d->radius)
        {
            qWarning() << "Length of capsule must exceed its diameter"
                          << " - correcting length.";
            d->length = 2 * d->radius;
        }

        qreal diameter = d->radius+d->radius;
        qreal cylinderHeight = d->length - diameter;
        qreal offset = cylinderHeight/2.0;

        builder << QGL::Faceted;
        QGLSceneNode *s = 0;
        s = builder.newNode();
        s->setObjectName("Cylinder");
        builder << QGLCylinder(diameter, diameter, cylinderHeight,
                               facets, layers, false, false);
        QMatrix4x4 translateMatrix;
        translateMatrix.translate(0, 0, -offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        s = builder.newNode();
        s->setObjectName("LeftEndCap");
        builder << QGLDome(diameter, divisions, false);
        translateMatrix.setToIdentity();
        translateMatrix.rotate(180, 0, 1,0);
        translateMatrix.translate(0, 0, offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        s = builder.newNode();
        s->setObjectName("RightEndCap");
        builder << QGLDome(diameter, divisions, false);
        translateMatrix.setToIdentity();
        translateMatrix.translate(0, 0, offset);
        builder.currentNode()->setLocalTransform(translateMatrix);

        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);

        d->lodGeometry.insert(d->lod, geometry);
    }
    Q_ASSERT_X(geometry != 0, Q_FUNC_INFO, "Could not create/find geometry!");
    if (d->currentCapsule != geometry)
    {
        if (d->currentCapsule)
            d->topNode->removeNode(d->currentCapsule);
        d->topNode->addNode(geometry);
        d->currentCapsule = geometry;
    }

    if (!d->sceneSet)
    {
        setScene(new CapsuleScene(d->topNode));
        d->sceneSet = true;
    }
}

QT_END_NAMESPACE

