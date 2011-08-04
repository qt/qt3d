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

#include "qdeclarativeitem3d.h"
#include "qdeclarativeviewport.h"
#include "qdeclarativemesh.h"
#include "qdeclarativeeffect.h"

#include "qgllightparameters.h"
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglview.h"
#include "qgraphicstransform3d.h"

#include <QtGui/qevent.h>
#include <QtDeclarative/qdeclarativecontext.h>

/*!
    \qmlclass Item3D QDeclarativeItem3D
    \brief The Item3D item encapsulates 3D objects and contains all of the properties and methods needed for simple 3D operations.
    part of a QML/3d script.
    \since 4.8
    \ingroup qt3d::qml3d

    \section1 Simple 3D Object Definition

    The most basic use case for the Item3D class is the creation and display of a
    single simple item in the 3D environment.

    Many such items within a 3D environment are defined as a single logical component
    mesh which is treated as a stand-alone object for the purposes of rotation,
    scaling, and user interaction via "picking".

    Such an object can easily be defined in QML using the following code:

    \code
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        effect: Effect {}
        cullFaces: "CullBackFaces"
    }
    \endcode

    This simple code will create a 3D item based on the \i teapot.bez mesh using
    back-face culling.

    Notice that in this case the effect and mesh are defined within the body of
    the item definition.  Where there is little complexity, this method of
    defining items simplifies the resultant code and makes modification of
    the QML easier.

    \section1 Embedding Item3D Objects

    Consider the following:

    \code
    Item3D {
        id: cup
        mesh:  Mesh { source: "teacup.bez" }
        effect: Effect {}
        cullFaces: "CullBackFaces"

        Item3D {
            id: saucer
            mesh: Mesh { source: "saucer.bez" }
            cullFaces: "CullBackFaces"
        }

        position: Qt.vector3d(10, -10, -10)
    }
    \endcode

    This demonstrates the capability of embedding one Item3D within another.  In
    this case the \i saucer item is a child of the \i cup item.

    All transformations applied to the parent item are also applied to the child,
    so in this case both the cup and saucer will be translated based on the
    position vector defined in the cup item's definition.

    In this case any additional transformations applied to the child item will not
    affect the parent, and are local only to that item (and to its children if any
    exist).

    This allows a user to group together items logically so that transformations and
    user interactions can be applied to groups of objects as if they were a whole.

    \section1 Using Sub-nodes of 3D Objects

    In more complex applications the user may wish to load a complex mesh
    which is made up of a number of components or nodes which may be organised
    into a tree like structure.  In this case they may wish to interact with,
    animate, or otherwise modify individual sub-nodes of a mesh.

    Item3D leverages the existing \bold {Qt Object Model} in order to allow QML/3d users
    this type of control over their 3D items.

    Consider the following QML script:

    \code
    Item3D {
        id: helicoptor
        mesh:  helicoptorMesh
        effect: Effect {}
        cullFaces: "CullBackFaces"

        transform: [
            Rotation3D {
                id: rotate1
                angle: 5
                axis: Qt.vector3d(1, 0, 0)
            },
            Rotation3D {
                id: rotate2
                angle: 5
                axis: Qt.vector3d(0, 1, 0)
            },
            Rotation3D {
                id: rotate3
                angle: 45
                axis: Qt.vector3d(0, 0, 1)
            }
        ]

        Item3D {
            id: rotor
            property bool spin: false
            meshNode: "rotorBladesNode"
            Item3D {meshNode: "rotorHubNode"}

            transform: [
                Rotation3D {
                    id: rotateBlades
                    angle: 0
                    axis: Qt.vector3d(0, 0, 1)
                }
            ]

            onClicked: { rotor.spin=true }

            SequentialAnimation {
                running: rotor.spin
                NumberAnimation {
                    target: rotateBlades
                    property: "angle"
                    to: 360.0
                    duration: 750
                    easing.type:"OutQuad"
                }
                onCompleted: rotor.spin = false
            }
        }
    }

    Mesh {
        id: helicoptorMesh
        source: "bellUH1.3ds"
    }
    \endcode

    Obviously this example is much more complex both in structure and behaviour. In
    this case the mesh describes a \i .3ds file of a helicoptor, which is broken down
    discrete sub-components (engine nacelles, rotor, rotor hub, etc), which the user
    may wish to modify or animate individually.

    Each child item in this case does not have a mesh explicitly defined, but rather
    inherits the mesh from the parent.  However each child item does define a mesh node
    which is part of the parent mesh.

    All transformations carried out on the parent item will also be applied to the child.

    Child items can, as shown here, have their own \i local transformations and user
    interactions applied.  These will be applied only to the node of the mesh which
    is defined for that item.  In cases where the mesh is defined heirarchically as a
    tree of nodes, this transformation will therefore be applied to all items in that
    tree which are children of the defined node.

    Likewise if the user explicitly declares a child item, such as has been done here with
    the \i rotorHubNode, then the transformations will apply to this item as well (and
    its children, and so on).

    It should be noted that no support is currently provided for skeleton animation or
    kinematic control of items.  This is left to the user to implement as required.

    \section1 Using QML Data Models With Item3D

    QDeclarativeItem3D supports standard \l
    {http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qml-data-models}
    {QML Data Models} with a few caveats.

    QDeclarativeItem3D derives from QtDeclarativeItem, and interacts with
    the \l{http://doc.qt.nokia.com/4.7/qml-component.html}{Component} element
    normally.  However, there is a delay between between removing an item from
    a model and the cleaning up the corresponding Item3D, so it is recommended
    that Item3D based delegates hide themselves when their index is
    -1 as shown in the photoroom example:

    \snippet quick3d/photoroom/qml/photoroom.qml 1

    However Item3D does not use the width or height properties, so most
    positioners and views will not work.  Use a
    \l{http://doc.qt.nokia.com/4.7/qml-repeater.html}{Repeater} element to
    generate Item3Ds from model data.  For example:

    \snippet quick3d/photoroom/qml/photoroom.qml 2

    Models can be used normally, so
\l{http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#listmodel}{ListModel},
\l{http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qstringlist}{QStringList}
    etc. work just like they would with two dimensional Items.  For example:

    \snippet quick3d/photoroom/qml/photoroom.qml 0

    \sa{http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qml-data-models}{QML Data Models}
*/



QT_BEGIN_NAMESPACE

class QDeclarativeItem3DPrivate
{
public:
    QDeclarativeItem3DPrivate(QDeclarativeItem3D *_item)
        : item(_item)
        , viewport(0)
        , position(0.0f, 0.0f, 0.0f)
        , pivot(0.0f,0.0f,0.0f)
        , usePivot(false)
        , scale(1.0f)
        , mesh(0)
        , effect(0)
        , requireBlockingEffectsCheck(false)
        , light(0)
        , objectPickId(-1)
        , cullFaces(QDeclarativeItem3D::CullDisabled)
        , sortChildren(QDeclarativeItem3D::DefaultSorting)
        , inheritEvents(false)
        , isEnabled(true)
        , isInitialized(false)
        , mainBranchId(0)
        , componentComplete(false)
    {
    }
    ~QDeclarativeItem3DPrivate();

    QDeclarativeItem3D *item;
    QDeclarativeViewport *viewport;
    QVector3D position;
    QVector3D pivot;
    bool usePivot;
    qreal scale;
    QDeclarativeMesh *mesh;
    QDeclarativeEffect *effect;
    bool requireBlockingEffectsCheck;
    QGLLightParameters *light;
    int objectPickId;
    QDeclarativeItem3D::CullFaces cullFaces;
    QDeclarativeItem3D::SortMode sortChildren;

    bool inheritEvents;
    bool isEnabled;
    bool isInitialized;
    int mainBranchId;
    QString meshNode;

    // data property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);

    // resources property
    static QObject *resources_at(QDeclarativeListProperty<QObject> *, int);
    static void resources_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int resources_count(QDeclarativeListProperty<QObject> *);

    // transform property
    static int transform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    static void transform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *);
    static QGraphicsTransform3D *transform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int);
    static void transform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    QList<QGraphicsTransform3D *> transforms;


    // pretransform property
    static int pretransform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    static void pretransform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *);
    static QGraphicsTransform3D *pretransform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int);
    static void pretransform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    QList<QGraphicsTransform3D *> pretransforms;

    // transform convenience functions
    QMatrix4x4 localTransforms() const;
    QMatrix4x4 localToWorldMatrix() const;
    QMatrix4x4 worldToLocalMatrix() const;
    bool componentComplete;
};

QDeclarativeItem3DPrivate::~QDeclarativeItem3DPrivate()
{
}

int QDeclarativeItem3DPrivate::transform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{
    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        return object->d->transforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void QDeclarativeItem3DPrivate::transform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *item)
{
    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    QList<QGraphicsTransform3D *> *ptrans;
    if (object)
    {
        ptrans = &object->d->transforms;

        //We now need to connect the underlying transform so that any change will update the graphical item.
        if (!ptrans->contains(item)) {
            ptrans->append(item);
            QObject::connect(item, SIGNAL(transformChanged()),
                             object, SLOT(update()));
        }
    }
    else
        qWarning()<<"Warning: could not find Item3D to add transformation to.";
}

QGraphicsTransform3D *QDeclarativeItem3DPrivate::transform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int idx)
{
   QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        return object->d->transforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void QDeclarativeItem3DPrivate::transform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{

    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        object->d->transforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}

int QDeclarativeItem3DPrivate::pretransform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{
    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        return object->d->pretransforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void QDeclarativeItem3DPrivate::pretransform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *item)
{
    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    QList<QGraphicsTransform3D *> *ptrans;
    if (object)
    {
        ptrans = &object->d->pretransforms;

        //We now need to connect the underlying transform so that any change will update the graphical item.
        if (!ptrans->contains(item)) {
            ptrans->append(item);
            QObject::connect(item, SIGNAL(transformChanged()),
                             object, SLOT(update()));
        }
    }
    else
        qWarning()<<"Warning: could not find Item3D to add transformation to.";
}

QGraphicsTransform3D *QDeclarativeItem3DPrivate::pretransform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int idx)
{
   QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        return object->d->pretransforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void QDeclarativeItem3DPrivate::pretransform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{

    QDeclarativeItem3D *object = qobject_cast<QDeclarativeItem3D *>(list->object);
    if (object) {
        object->d->pretransforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}



void QDeclarativeItem3DPrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    // This function is called by the QML runtime to assign children to
    // an item3d.  The object 'o' is the new child, and prop->object is the
    // Item3d that is to be the parent.

    // Either way we're going to call something like setParent(prop->object)
    // we're just determining whether to use the QDeclarativeItem or QObject
    // version.

    // The primary purpose of this function is to divide new children into
    // renderable qml Items and non-renderable QObject derived resources.
    // We want to accept all Items, and and simply ignore non-3d items
    // during drawing.

    // It is important that we imitate this behaviour of non-3d
    // QDeclarativeItems so view items will assign dynamically created objects
    // to the Item3d and make them available for drawing.

    QDeclarativeItem *i = qobject_cast<QDeclarativeItem *>(o);
    if (i)
        i->setParentItem(static_cast<QDeclarativeItem3D *>(prop->object));
    else
        o->setParent(static_cast<QDeclarativeItem3D *>(prop->object));
}


QObject *QDeclarativeItem3DPrivate::resources_at(QDeclarativeListProperty<QObject> *prop, int index)
{
    QObjectList children = prop->object->children();
    if (index < children.count())
        return children.at(index);
    else
        return 0;
}

void QDeclarativeItem3DPrivate::resources_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    o->setParent(prop->object);
}

int QDeclarativeItem3DPrivate::resources_count(QDeclarativeListProperty<QObject> *prop)
{
    return prop->object->children().count();
}

/*!
    \internal
    Applies position, scale and rotation transforms for this item3d to matrix
    \a m
*/
QMatrix4x4 QDeclarativeItem3DPrivate::localTransforms() const
{
    QMatrix4x4 m;
    m.translate(position);
    int transformCount = transforms.count();
    if (transformCount>0) {
        // The transformations are applied in reverse order of their
        // lexical appearance in the QML file.
        for (int index = transformCount - 1; index >= 0; --index) {
            transforms.at(index)->applyTo(&m);
        }
    }
    if (scale != 1.0f)
        m.scale(scale);
    transformCount = pretransforms.count();
    if (transformCount>0) {
        // Pre-transforms for orienting the model.
        for (int index = transformCount - 1; index >= 0; --index) {
            pretransforms.at(index)->applyTo(&m);
        }
    }
    return m;
}


/*!
    \internal
*/
QDeclarativeItem3D::QDeclarativeItem3D(QObject *parent)
    : QDeclarativeItem(0)
{
    d = new QDeclarativeItem3DPrivate(this);

    QDeclarativeItem *itemParent = qobject_cast<QDeclarativeItem *>(parent);
    if (itemParent) {
        setParentItem(itemParent);
    } else {
        setParent(parent);
    }

    // TODO: Handle QDeclarativeItem3D case
}

/*!
    \internal
*/
QDeclarativeItem3D::~QDeclarativeItem3D()
{
    delete d;
}



/*!
    \qmlproperty vector3D Item3D::position

    The position in 3D space of this item.  The default value for this
    property is (0, 0, 0).

    \sa x, y, z
*/

QVector3D QDeclarativeItem3D::position() const
{
    return d->position;
}

void QDeclarativeItem3D::setPosition(const QVector3D& value)
{
    d->position = value;
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::x

    The x position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, y, z
*/

qreal QDeclarativeItem3D::x() const
{
    return d->position.x();
}

void QDeclarativeItem3D::setX(qreal value)
{
    d->position.setX(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::y

    The y position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, x, z
*/

qreal QDeclarativeItem3D::y() const
{
    return d->position.y();
}

void QDeclarativeItem3D::setY(qreal value)
{
    d->position.setY(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::z

    The z position of this item in 3D space.  The default value for
    this property is 0.

    \sa position, x, y
*/

qreal QDeclarativeItem3D::z() const
{
    return d->position.z();
}

void QDeclarativeItem3D::setZ(qreal value)
{
    d->position.setZ(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::scale

    The scaling factor to apply to the item after the transformations
    from the Item3D::transform property.  The default value for this
    property is 1.

    \sa transform
*/

qreal QDeclarativeItem3D::scale() const
{
    return d->scale;
}

void QDeclarativeItem3D::setScale(qreal value)
{
    d->scale = value;
    emit scaleChanged();
    update();
}

/*!
    \qmlproperty list<Transform> Item3D::transform

    Generally objects in 3D space will have undergone some number
    of 3D transformation prior to display.  Examples of such transformations
    include rotations about the x, y, and z axes, translation, and so on.

    Each Item3D maintains a list of transforms to apply to it through this
    property.  In scripting terms a transform can be applied as follows:

    \code
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        transform: [
            Rotation3D {
                id: teapot_rotate1
                angle: 0
                axis: Qt.vector3d(0, 1, 0)
            },
            Rotation3D {
                id: teapot_rotate2
                angle: 0
                axis: Qt.vector3d(0, 0, 1)
            }
        ]
    }
    \endcode

    In this example we have two transformations in our list - a rotation around
    the y axis (\c {teapot_rotate1}), and a rotation about the z axis (\c {teapot_rotate2}).

    These transformations can be accessed via standard QML scripting methods to achieve
    animations and other effects.

    By default this list of transformations is empty.

    \sa Rotation3D, Scale3D, Translation3D, scale, position, pretransform
*/


QDeclarativeListProperty<QGraphicsTransform3D> QDeclarativeItem3D::transform()
{
    return QDeclarativeListProperty<QGraphicsTransform3D>(this, 0, d->transform_append, d->transform_count,
                                               d->transform_at, d->transform_clear);
}

/*!
    \qmlproperty list<Transform> Item3D::pretransform

    The transformations to apply before all others.

    When a model is loaded from an external source such as a 3D
    modeling package, it is usually in an unconventional orientation
    and position.  The first step is to rotate, scale, and translate
    it to make it suitable for use as a QML object.

    The purpose of the \c pretransform property is to perform such
    "model correction" transformations before \c scale, \c transform,
    and \c position are applied to place the model in its final
    orientation and position in the QML application.

    By default this list of transformations is empty.

    \sa transform, scale, position
*/

QDeclarativeListProperty<QGraphicsTransform3D> QDeclarativeItem3D::pretransform()
{
    return QDeclarativeListProperty<QGraphicsTransform3D>(this, 0, d->pretransform_append, d->pretransform_count,
                                               d->pretransform_at, d->pretransform_clear);
}

/*!
    \qmlproperty bool Item3D::inheritEvents

    Users are able to interact with 3d items in a scene through (for example) the
    use of the mouse.  These, and other, Qt events can be captured by an \l Item3D using the
    same underlying QObject architecture shared by all of Qt.

    Often a user will only want an item to capture mouse events for itself, leaving
    child items to handle their mouse events locally.  Under many circumstances, however, it
    is necessary for a parent object to collect all mouse events for itself and its child
    items.  Usually this inheritance of events is only defined at initialisation for an \l Item3D

    The inheritEvents property, however, is a simple boolean property which provides a mechanism
    for both initialisation time and programmatic modification of this.

    Setting the property to true connects the signals for all child items to the appropriate
    signals for the item itself.  Conversely setting the property to false disconnects the
    events.

    The default value for this property is false.
*/
bool QDeclarativeItem3D::inheritEvents() const
{
    return d->inheritEvents;
}

void QDeclarativeItem3D::setInheritEvents(bool inherit)
{
    d->inheritEvents = inherit;

    //Generally we would only want to
    if (inherit)
    {
        for (int index = 0; index < children().size(); ++index) {
            QDeclarativeItem3D *subItem = qobject_cast<QDeclarativeItem3D *>(children().at(index));
            if (subItem)
            {
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for a group of children.
                QObject::connect(subItem, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::connect(subItem, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::connect(subItem, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::connect(subItem, SIGNAL(released()), this, SIGNAL(released()));
                QObject::connect(subItem, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::connect(subItem, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }
        }
    }
    else
    {
        for (int index = 0; index < children().size(); ++index) {
            QDeclarativeItem *subItem = qobject_cast<QDeclarativeItem *>(children().at(index));
            if (subItem)
            {
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for a group of children.
                QObject::disconnect(subItem, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::disconnect(subItem, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::disconnect(subItem, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::disconnect(subItem, SIGNAL(released()), this, SIGNAL(released()));
                QObject::disconnect(subItem, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::disconnect(subItem, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }
        }
    }
}

/*!
    \qmlproperty Mesh Item3D::mesh

    Objects in most 3D environments are almost invariably defined as meshes - sets of
    vertices which when linked as polygons form a recognisable 3D object.  QtQuick3D currently
    supports a number of these \i {scene formats}, including \i {.obj} file, bezier patches
    \i {(.bez)}, and \i {.3ds} files.

    These meshes are abstracted into the \l Mesh class, which is defined for
    an \l Item3D through this property.

    The default value for this property is null, so a mesh must be defined in
    order for the item to be displayed

    \sa effect
*/

QDeclarativeMesh *QDeclarativeItem3D::mesh() const
{
    return d->mesh;
}

void QDeclarativeItem3D::setMesh(QDeclarativeMesh *value)
{
    if (d->mesh != value)
    {
        if (d->mesh) {
            if (!d->mesh->deref())
                delete d->mesh;
        }

        d->mesh = value;
        //always start off pointing to the default scene mesh object.
        d->mainBranchId = 0;

        if (value) {
            d->mesh->ref();
            connect(value, SIGNAL(dataChanged()), this, SIGNAL(meshChanged()));
            connect(value, SIGNAL(dataChanged()), this, SLOT(update()));
            d->requireBlockingEffectsCheck = true;
        }

        emit meshChanged();

        update();
    }
}

/*!
    \qmlproperty Effect Item3D::effect

    QML 3D items support the use of effects for modifying the display
    of items - texture effects, fog effects, material effects, and so on.

    The exact effects correlated with an item are set using this property.

    The default value for this propertly is null, and so an effect - even an
    empty one - must be defined if the mesh does not contain its own effects.

    \sa Effect, mesh
*/
QDeclarativeEffect *QDeclarativeItem3D::effect() const
{
    return d->effect;
}

void QDeclarativeItem3D::setEffect(QDeclarativeEffect *value)
{
    if (d->effect == value)
        return;
    if (d->effect)
        disconnect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
    d->effect = value;
    if (d->effect)
    {
        connect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
        d->requireBlockingEffectsCheck = true;
    }
    emit effectChanged();
    update();
}

/*!
    \qmlproperty Light Item3D::light

    This property defines an item-specific light that will be used
    intead of Viewport::light for rendering this item and its children
    if the value is not null.

    \sa Viewport::light
*/

QGLLightParameters *QDeclarativeItem3D::light() const
{
    return d->light;
}

void QDeclarativeItem3D::setLight(QGLLightParameters *value)
{
    if (d->light != value) {
        if (d->light) {
            disconnect(d->light, SIGNAL(lightChanged()),
                       this, SLOT(update()));
        }
        d->light = value;
        if (d->light) {
            connect(d->light, SIGNAL(lightChanged()),
                    this, SLOT(update()));
        }
        emit lightChanged();
        update();
    }
}

/*!
    \qmlproperty list<Item3D> Item3D::children
    \qmlproperty list<Object> Item3D::resources

    The children property contains a list of all QDeclarativeItem derived
    child items for this item.  This provides logical grouping of items in a
    scene. Transformations that are applied to this item will also affect
    child items.  Note that children that are not derived from
    QDeclarativeItem3D will not be rendered at draw time, but will interact
    normally otherwise (e.g. parenting, signal passing etc).  Use a
    qobject_cast if you need to check whether a child is an Item3D.

    The resources property holds all other children that do not
    directly inherit from QDeclarativeItem, such as effects, meshes, and
    other supporting objects.

    Normally it isn't necessary to assign to the children or resources
    properties directly as the QML syntax will take care of the
    assignment depending upon the object's type.

    \sa transform
*/

QDeclarativeListProperty<QObject> QDeclarativeItem3D::resources()
{
    return QDeclarativeListProperty<QObject>(this, 0, QDeclarativeItem3DPrivate::resources_append,
                                             QDeclarativeItem3DPrivate::resources_count,
                                             QDeclarativeItem3DPrivate::resources_at);
}



/*!
    \qmlproperty list<Object> Item3D::data

    This property exists to allow future expansion of the Item3D class to
    include additional data and resources.  Currently there is no underlying
    implementation for this.
*/
QDeclarativeListProperty<QObject> QDeclarativeItem3D::data()
{
    return QDeclarativeListProperty<QObject>(this, 0, QDeclarativeItem3DPrivate::data_append);
}

/*!
    \qmlproperty enumeration Item3D::cullFaces

    This property defines the culling method to be use on fragments
    within the item's mesh.  Culling of an item in 3D space can be
    carried out in a number of ways:

    \list
    \o CullDisabled Do not use culling.  This is the default value.
    \o CullFrontFaces Cull the front faces of the object.
    \o CullBackFaces Cull the back faces of the object.
    \o CullAllFaces Cull all faces of the object.
    \o CullClockwise Cull faces based on clockwise winding of vertices.
    \endlist
*/
QDeclarativeItem3D::CullFaces QDeclarativeItem3D::cullFaces() const
{
    return d->cullFaces;
}

void QDeclarativeItem3D::setCullFaces(QDeclarativeItem3D::CullFaces value)
{
    if (d->cullFaces != value) {
        d->cullFaces = value;
        emit meshChanged();
    }
}

/*!
    \qmlproperty enumeration Item3D::sortChildren

    This property defines the sorting mode to apply to child \l Item3D
    elements when they are drawn.

    \list
    \o DefaultSorting No explicit sorting of the children - draw them in
       whatever order is convenient for the system.  The system may apply
       its own sorting, grouping similar materials to improve performance.
       This is the default.
    \o BackToFront Sort the children to draw them in back-to-front
       order of their \l position, overriding any system-supplied sorting.
       BackToFront is useful when the children are partially transparent
       and must be drawn in back-to-front order for correct rendering.
    \endlist
*/
QDeclarativeItem3D::SortMode QDeclarativeItem3D::sortChildren() const
{
    return d->sortChildren;
}

void QDeclarativeItem3D::setSortChildren(QDeclarativeItem3D::SortMode mode)
{
    if (d->sortChildren != mode) {
        d->sortChildren = mode;
        emit sortChildrenChanged();
    }
}

/*!
    \internal
    Performs the actual drawing of the Item3D using \a painter.

    If the item is set to object picking mode this includes all of the infrastructure needed
    to support picking of objects.

    The basic premise of the draw function should be familiar to users of OpenGL or similar
    graphics libraries.  Essentially it is a stepwise progress through the following stages:

    \list
    \i 1. Iterate through the child objects of the item and set all lighting parameters found.
    \i 2. Set up culling mode in the painter.
    \i 3. Set effects if they exist.
    \i 4. Set all local model view transformations for this item.
    \i 5. Draw this item.
    \i 6. Iterate through the child objects of the item and draw all child items.
    \i 7. Unset the appropriate parameters and states.
    \endlist

    \sa drawItem()
*/

void QDeclarativeItem3D::draw(QGLPainter *painter)
{
    // Bail out if this item and its children have been disabled.
    if (!d->isEnabled)
        return;
    if (!d->isInitialized)
        initialize(painter);

    int prevId = painter->objectPickId();
    painter->setObjectPickId(d->objectPickId);

    //Lighting
    const QGLLightParameters *currentLight = 0;
    QMatrix4x4 currentLightTransform;
    if (d->light) {
        currentLight = painter->mainLight();
        currentLightTransform = painter->mainLightTransform();
        painter->setMainLight(d->light);
    }

    //Culling
    if ((d->cullFaces & ~CullClockwise) == CullDisabled) {
        glDisable(GL_CULL_FACE);
    } else if (d->cullFaces & CullClockwise) {
        glFrontFace(GL_CW);
        glCullFace(GLenum(d->cullFaces & ~CullClockwise));
        glEnable(GL_CULL_FACE);
    } else {
        glFrontFace(GL_CCW);
        glCullFace(GLenum(d->cullFaces));
        glEnable(GL_CULL_FACE);
    }

    // Blending change for the effect.
    bool viewportBlend = d->viewport ? d->viewport->blending() : false;
    bool effectBlend = d->effect ? d->effect->blending() : viewportBlend;
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    //Effects
    if (d->effect)
        d->effect->enableEffect(painter);

    //Local and Global transforms

    //1) Item Transforms
    painter->modelViewMatrix().push();
    painter->modelViewMatrix() *= d->localTransforms();

    //Drawing
    drawItem(painter);

    // Find all 3d children for drawing
    QList<QDeclarativeItem3D *> list;;
    foreach (QObject* o, children())
    {
        if (QDeclarativeItem3D *item3d = qobject_cast<QDeclarativeItem3D*>(o))
        {
            list.append(item3d);
        }
    }

    if (d->sortChildren == QDeclarativeItem3D::BackToFront) {
        // Collect up the transformed z positions of all children.
        QList<qreal> zlist;
        QMatrix4x4 mv = painter->modelViewMatrix();
        for (int index = 0; index < list.size(); ++index) {
            QVector3D position = list.at(index)->position();
            zlist.append(mv.map(position).z());
        }

        // Sort the item list (Caution: really dumb sort algorithm).
        for (int i = 0; i < list.size() - 1; ++i) {
            for (int j = i + 1; j < list.size(); ++j) {
                if (zlist.at(i) > zlist.at(j)) {
                    qSwap(list[i], list[j]);
                    qSwap(zlist[i], zlist[j]);
                }
            }
        }
    }
    for (int index = 0; index < list.size(); ++index)
        list.at(index)->draw(painter);

    //Unset parameters for transforms, effects etc.
    painter->modelViewMatrix().pop();

    if (d->effect)
        d->effect->disableEffect(painter);
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glDisable(GL_BLEND);
        else
            glEnable(GL_BLEND);
    }
    if (d->cullFaces != CullDisabled)
        glDisable(GL_CULL_FACE);
    if (d->light)
        painter->setMainLight(currentLight, currentLightTransform);
    painter->setObjectPickId(prevId);
}

/*!
    \internal
*/
void QDeclarativeViewport::setItemViewport(QDeclarativeItem3D *item)
{
    item->d->viewport = this;
}

/*!
    \internal
    The process of initialising an /l Object3d is a critical step, particularly in
    complex scenes.  This function initialises the item in \a viewport, and using \a painter.

    During the initialisation process objects are registered as being \i pickable (ie. able
    to be clicked on with the mouse.

    Additionally, in the case of \l Item3D objects which refer to sub-nodes of a mesh, this
    function performs all of the splitting of meshes into sub-branches ready for local
    control by the item.
*/
void QDeclarativeItem3D::initialize(QGLPainter *painter)
{
    if (d->isInitialized) return;

    if (!d->viewport)
    {
        if (QDeclarativeItem3D* parentItem =
                qobject_cast<QDeclarativeItem3D*>(parent()))
        {
            d->viewport = parentItem->d->viewport;
            Q_ASSERT(d->viewport);
        }
    }

    d->objectPickId = d->viewport->registerPickableObject(this);

    for (int index = 0; index < children().size(); ++index) {
        QDeclarativeItem3D *item = qobject_cast<QDeclarativeItem3D *>(children().at(index));
        if (item) {
            //Event inheritance is generally only declared at initialization, but can also be done at runtime
            //if the user wishes (though not recommended).
            if (inheritEvents()) {
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for its children.
                QObject::connect(item, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::connect(item, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::connect(item, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::connect(item, SIGNAL(released()), this, SIGNAL(released()));
                QObject::connect(item, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::connect(item, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }
            //if the item has no mesh of its own and no meshnode is declared we give it the mesh from the current item.
            if (!item->mesh() && !item->meshNode().isEmpty()) {
                item->setMesh(mesh());
            }

            d->viewport->setItemViewport(item);
            item->initialize(painter);
        }
    }
    d->isInitialized = true;
}

void QDeclarativeItem3D::componentComplete()
{
    QDeclarativeItem::componentComplete();
    d->componentComplete = true;

    // Now that we have all the mesh and subnode information we need, it's time to setup the mesh scene objects.
    if (mesh() && !meshNode().isEmpty()) {
        int branchNumber = mesh()->createSceneBranch(meshNode());
        if (branchNumber>=0) {
            d->mainBranchId = branchNumber;
        }
        else {
            qWarning()<< "3D item initialization failed: unable to find the specified mesh-node. Defaulting to default node.";
            d->mainBranchId = 0;
        }
    }
    update();
}

/*!
    \internal
    The \c drawItem function performs the actual drawing of the mesh branch which corresponds
    to the section of the mesh being drawn by the \l Item3D to a specific \a painter.
*/
void QDeclarativeItem3D::drawItem(QGLPainter *painter)
{
    if (d->mesh)
    {
        d->mesh->draw(painter, d->mainBranchId);
    }
}


/*!
    Calculates and returns a matrix that transforms local coordinates into
    world coordinates (i.e. coordinates untransformed by any item3d's
    transforms).
*/
QMatrix4x4 QDeclarativeItem3DPrivate::localToWorldMatrix() const
{
    QMatrix4x4 result;

    result = localTransforms() * result;
    QDeclarativeItem3D *anscestor = qobject_cast<QDeclarativeItem3D *>(item->parent());
    while (anscestor)
    {
        result = anscestor->d->localTransforms() * result;
        anscestor = qobject_cast<QDeclarativeItem3D *>(anscestor->parent());
    }
    return result;
}

/*!
    Calculates and returns a matrix that transforms world coordinates into
    coordinates relative to this Item3D.
*/
QMatrix4x4 QDeclarativeItem3DPrivate::worldToLocalMatrix() const
{
    bool inversionSuccessful;
    QMatrix4x4 result = localToWorldMatrix().inverted(&inversionSuccessful);
    if (inversionSuccessful)
        return result;
    qWarning() << "QDeclarativeItem3D - matrix inversion failed trying to generate worldToLocal Matrix";
    return QMatrix4x4();
}

/*!
    Returns the position of a local \a point in world space (i.e. not
    transformed by this item, it's parents, the camera etc).
*/
QVector3D QDeclarativeItem3D::localToWorld(const QVector3D &point) const
{
    return d->localToWorldMatrix() * point;
}

/*!
    Returns the position of a point in world space in local coordinates.
*/
QVector3D QDeclarativeItem3D::worldToLocal(const QVector3D &point) const
{
    return d->worldToLocalMatrix() * point;
}

/*!
    \internal
    This function handles the standard mouse events for the item as contained in \a e.

    Returns the boolean value of the regular QObject::event() function.oo
*/

bool QDeclarativeItem3D::event(QEvent *e)
{
    // Convert the raw event into a signal representing the user's action.
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
            emit pressed();
    } else if (e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton) {
            emit released();
            if (me->x() >= 0)   // Positive: inside object, Negative: outside.
                emit clicked();
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    } else if (e->type() == QEvent::Enter) {
        emit hoverEnter();
    } else if (e->type() == QEvent::Leave) {
        emit hoverLeave();
    }
    return QObject::event(e);
}

/*!
    \qmlproperty string Item3D::meshNode

    This property is a simple string which refers to the node in the \l Mesh object which
    is associated with this \l Item3D.

    \sa mesh
*/
QString QDeclarativeItem3D::meshNode() const
{
    return d->meshNode;
}

void QDeclarativeItem3D::setMeshNode(const QString &node)
{
    //the actual instantiation of the node as the mesh itself is undertaken in the initialize function.
    d->meshNode = node;
}

/*!
    \internal
    Update the \l Viewport with which this item is associated.
*/
void QDeclarativeItem3D::update()
{
    if (d->requireBlockingEffectsCheck && d->effect && d->mesh && d->componentComplete)
    {
        QGLSceneNode *n = 0;
        if (!meshNode().isEmpty())
            n = d->mesh->getSceneObject(meshNode());
        if (!n)
            n = d->mesh->getSceneBranch(d->mainBranchId);
        if (!n)
        {
            n = d->mesh->getSceneObject();
        }
        if (n)
        {
            QList<QGLSceneNode*> k = n->allChildren();
            k.prepend(n);
            for (int i = 0; i < k.size(); ++i)
            {
                // If the effect has a texture, make sure the mesh does too
                bool hasTexture = (!d->effect->texture().isEmpty() ||
                                   (!d->effect->textureImage().isNull()));
                bool missingTextureCoordinates =
                        k.at(i)->geometry().hasField(QGL::Position) &&
                        !k.at(i)->geometry().hasField(QGL::TextureCoord0);
                if ( hasTexture && missingTextureCoordinates )
                {
                    qWarning() << "QGLSceneNode" << k.at(i)->objectName() << "from" << d->mesh->source() << "is missing texture coordinates.  Dummy coordinates are being generated, which may take some time.";
                    k.at(i)->geometry().generateTextureCoordinates();
                }

                QGLSceneNode* sceneObject;
                if (!this->meshNode().isEmpty())
                {
                    sceneObject = d->mesh->getSceneObject(meshNode());
                } else
                    sceneObject = d->mesh->getSceneObject();

                if (sceneObject)
                {
                    QList<QGLSceneNode*> k = n->allChildren();
                    k.prepend(n);
                    if (this->effect())
                    {
                        for (int i = 0; i < k.size(); ++i)
                        {
                            this->effect()->applyTo(k.at(i));
                        }
                    }
                }
            }
        }
        d->requireBlockingEffectsCheck = false;
    }
    if (d->viewport)
        d->viewport->update3d();
}

/*!
    \qmlproperty bool Item3D::enabled

    This property should be set to true to enable the drawing
    of this item and its children.  Set this property to false
    to disable drawing.  The default value is true.

    \sa mesh
*/
bool QDeclarativeItem3D::isEnabled() const
{
    return d->isEnabled;
}

void QDeclarativeItem3D::setEnabled(bool value)
{
    if (d->isEnabled != value) {
        d->isEnabled = value;
        emit enabledChanged();
    }
}

/*!
  \qmlsignal Item3D::onClicked()

  This signal is emitted when the item is clicked.  Picking must be enabled for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onDoubleClicked()

  This signal is emitted when the item is double clicked.  Picking must be enabled for this to have any effect.

*/


/*!
  \qmlsignal Item3D::onPressed()

  This signal is emitted when the item detects a mouse-button-down event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onReleased()

  This signal is emitted when the item detects a mouse-button-released event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onHoverEnter()

  This signal is emitted when a mouseover of the item is detected.  It relies on object picking to be
  of use.
*/


/*!
  \qmlsignal Item3D::onHoverLeave()

  This signal is emitted when the mouseover of the item ceases.  It relies on object picking to be
  used.
*/

QT_END_NAMESPACE
