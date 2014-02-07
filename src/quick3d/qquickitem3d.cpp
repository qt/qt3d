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

#include "qquickitem3d.h"
#include "qquickviewport.h"
#include "qquickmesh.h"
#include "qquickeffect.h"

#include "qgllightparameters.h"
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglview.h"
#include "qgraphicstransform3d.h"

#include <QtGui/qevent.h>
#include <QtQml/qqmlcontext.h>
#include <QtQuick/qquickwindow.h>

/*!
    \qmltype Item3D
    \instantiates QQuickItem3D
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

    This simple code will create a 3D item based on the \e teapot.bez mesh using
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
    this case the \e saucer item is a child of the \e cup item.

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

    Item3D leverages the existing \b {Qt Object Model} in order to allow QML/3d users
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
    this case the mesh describes a \e .3ds file of a helicoptor, which is broken down
    discrete sub-components (engine nacelles, rotor, rotor hub, etc), which the user
    may wish to modify or animate individually.

    Each child item in this case does not have a mesh explicitly defined, but rather
    inherits the mesh from the parent.  However each child item does define a mesh node
    which is part of the parent mesh.

    All transformations carried out on the parent item will also be applied to the child.

    Child items can, as shown here, have their own \e local transformations and user
    interactions applied.  These will be applied only to the node of the mesh which
    is defined for that item.  In cases where the mesh is defined heirarchically as a
    tree of nodes, this transformation will therefore be applied to all items in that
    tree which are children of the defined node.

    Likewise if the user explicitly declares a child item, such as has been done here with
    the \e rotorHubNode, then the transformations will apply to this item as well (and
    its children, and so on).

    It should be noted that no support is currently provided for skeleton animation or
    kinematic control of items.  This is left to the user to implement as required.

    \section1 Using QML Data Models With Item3D

    QQuickItem3D supports standard \l
    {http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qml-data-models}
    {QML Data Models} with a few caveats.

    QQuickItem3D derives from QQuickItem, and interacts with
    the \l{http://doc.qt.nokia.com/4.7/qml-component.html}{Component} element
    normally.  However, there is a delay between between removing an item from
    a model and the cleaning up the corresponding Item3D, so it is recommended
    that Item3D based delegates hide themselves when their index is
    -1 as shown in the photoroom example:

    \snippet ../../examples/quick3d/photoroom/qml/photoroom.qml 1

    However Item3D does not use the width or height properties, so most
    positioners and views will not work.  Use a
    \l{http://doc.qt.nokia.com/4.7/qml-repeater.html}{Repeater} element to
    generate Item3Ds from model data.  For example:

    \snippet ../../examples/quick3d/photoroom/qml/photoroom.qml 2

    Models can be used normally, so
\l{http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#listmodel}{ListModel},
\l{http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qstringlist}{QStringList}
    etc. work just like they would with two dimensional Items.  For example:

    \snippet ../../examples/quick3d/photoroom/qml/photoroom.qml 0

    \sa {http://doc.qt.nokia.com/4.7/qdeclarativemodels.html#qml-data-models}{QML Data Models}
*/



QT_BEGIN_NAMESPACE

class QQuickItem3DPrivate
{
public:
    QQuickItem3DPrivate(QQuickItem3D *_item)
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
        , cullFaces(QQuickItem3D::CullDisabled)
        , sortChildren(QQuickItem3D::DefaultSorting)
        , inheritEvents(false)
        , isEnabled(true)
        , isInitialized(false)
        , mainBranchId(0)
        , componentComplete(false)
        , bConnectedToOpenGLContextSignal(false)
    {
    }
    ~QQuickItem3DPrivate();

    QQuickItem3D *item;
    QQuickViewport *viewport;
    QVector3D position;
    QVector3D pivot;
    bool usePivot;
    float scale;
    QQuickMesh *mesh;
    QQuickEffect *effect;
    bool requireBlockingEffectsCheck;
    QGLLightParameters *light;
    int objectPickId;
    QQuickItem3D::CullFaces cullFaces;
    QQuickItem3D::SortMode sortChildren;

    bool inheritEvents;
    bool isEnabled;
    bool isInitialized;
    int mainBranchId;
    QString meshNode;

    // data property
    static void data_append(QQmlListProperty<QObject> *, QObject *);

    // resources property
    static QObject *resources_at(QQmlListProperty<QObject> *, int);
    static void resources_append(QQmlListProperty<QObject> *, QObject *);
    static int resources_count(QQmlListProperty<QObject> *);
    static void resources_clear(QQmlListProperty<QObject> *);

    // transform property
    static int transform_count(QQmlListProperty<QQuickQGraphicsTransform3D> *list);
    static void transform_append(QQmlListProperty<QQuickQGraphicsTransform3D> *list, QQuickQGraphicsTransform3D *);
    static QQuickQGraphicsTransform3D *transform_at(QQmlListProperty<QQuickQGraphicsTransform3D> *list, int);
    static void transform_clear(QQmlListProperty<QQuickQGraphicsTransform3D> *list);
    QList<QQuickQGraphicsTransform3D *> transforms;


    // pretransform property
    static int pretransform_count(QQmlListProperty<QQuickQGraphicsTransform3D> *list);
    static void pretransform_append(QQmlListProperty<QQuickQGraphicsTransform3D> *list, QQuickQGraphicsTransform3D *);
    static QQuickQGraphicsTransform3D *pretransform_at(QQmlListProperty<QQuickQGraphicsTransform3D> *list, int);
    static void pretransform_clear(QQmlListProperty<QQuickQGraphicsTransform3D> *list);
    QList<QQuickQGraphicsTransform3D *> pretransforms;

    // animations property
    static int animations_count(QQmlListProperty<QQuickAnimation3D> *list);
    static void animations_append(QQmlListProperty<QQuickAnimation3D> *list, QQuickAnimation3D *);
    static QQuickAnimation3D *animations_at(QQmlListProperty<QQuickAnimation3D> *list, int);
    static void animations_clear(QQmlListProperty<QQuickAnimation3D> *list);
    QList<QQuickAnimation3D *> animations;

    // transform convenience functions
    QMatrix4x4 localTransforms() const;
    QMatrix4x4 localToWorldMatrix() const;
    QMatrix4x4 worldToLocalMatrix() const;
    bool componentComplete;

    bool bConnectedToOpenGLContextSignal;
};

QQuickItem3DPrivate::~QQuickItem3DPrivate()
{
}

int QQuickItem3DPrivate::transform_count(QQmlListProperty<QQuickQGraphicsTransform3D> *list)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->transforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void QQuickItem3DPrivate::transform_append(QQmlListProperty<QQuickQGraphicsTransform3D> *list, QQuickQGraphicsTransform3D *item)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    QList<QQuickQGraphicsTransform3D *> *ptrans;
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

QQuickQGraphicsTransform3D *QQuickItem3DPrivate::transform_at(QQmlListProperty<QQuickQGraphicsTransform3D> *list, int idx)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->transforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void QQuickItem3DPrivate::transform_clear(QQmlListProperty<QQuickQGraphicsTransform3D> *list)
{

    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        object->d->transforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}

int QQuickItem3DPrivate::pretransform_count(QQmlListProperty<QQuickQGraphicsTransform3D> *list)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->pretransforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void QQuickItem3DPrivate::pretransform_append(QQmlListProperty<QQuickQGraphicsTransform3D> *list, QQuickQGraphicsTransform3D *item)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    QList<QQuickQGraphicsTransform3D *> *ptrans;
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

QQuickQGraphicsTransform3D *QQuickItem3DPrivate::pretransform_at(QQmlListProperty<QQuickQGraphicsTransform3D> *list, int idx)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->pretransforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void QQuickItem3DPrivate::pretransform_clear(QQmlListProperty<QQuickQGraphicsTransform3D> *list)
{

    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        object->d->pretransforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}

int QQuickItem3DPrivate::animations_count(QQmlListProperty<QQuickAnimation3D> *list)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->animations.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for animations count.";
        return 0;
    }
}

void QQuickItem3DPrivate::animations_append(QQmlListProperty<QQuickAnimation3D> *list, QQuickAnimation3D *item)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    QList<QQuickAnimation3D *> *panim;
    if (object)
    {
        panim = &object->d->animations;
        if (!panim->contains(item)) {
            panim->append(item);
        }
    }
    else
        qWarning()<<"Warning: could not find Item3D to add animation to.";
}

QQuickAnimation3D* QQuickItem3DPrivate::animations_at(QQmlListProperty<QQuickAnimation3D> *list, int idx)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        return object->d->animations.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for animations";
        return 0;
    }
    return 0;
}

void QQuickItem3DPrivate::animations_clear(QQmlListProperty<QQuickAnimation3D> *list)
{
    QQuickItem3D *object = qobject_cast<QQuickItem3D *>(list->object);
    if (object) {
        object->d->animations.clear();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of animations";
}



void QQuickItem3DPrivate::data_append(QQmlListProperty<QObject> *prop, QObject *o)
{
    // This function is called by the QML runtime to assign children to
    // an item3d.  The object 'o' is the new child, and prop->object is the
    // Item3d that is to be the parent.

    // Either way we're going to call something like setParent(prop->object)
    // we're just determining whether to use the QQuickItem or QObject version.

    // The primary purpose of this function is to divide new children into
    // renderable qml Items and non-renderable QObject derived resources.
    // We want to accept all Items, and and simply ignore non-3d items
    // during drawing.

    // It is important that we imitate this behaviour of non-3d QQuickItems so
    // view items will assign dynamically created objects to the Item3d and
    // make them available for drawing.

    QQuickItem *i = qobject_cast<QQuickItem *>(o);
    if (i)
        i->setParentItem(static_cast<QQuickItem3D *>(prop->object));
    else
        o->setParent(static_cast<QQuickItem3D *>(prop->object));
}


QObject *QQuickItem3DPrivate::resources_at(QQmlListProperty<QObject> *prop, int index)
{
    QObjectList children = prop->object->children();
    if (index < children.count())
        return children.at(index);
    else
        return 0;
}

void QQuickItem3DPrivate::resources_append(QQmlListProperty<QObject> *prop, QObject *o)
{
    o->setParent(prop->object);
}

int QQuickItem3DPrivate::resources_count(QQmlListProperty<QObject> *prop)
{
    return prop->object->children().count();
}

void QQuickItem3DPrivate::resources_clear(QQmlListProperty<QObject> *property)
{
    QObjectList children = property->object->children();
    foreach (QObject *child, children)
        child->setParent(0);
}

/*!
    \internal
    Applies position, scale and rotation transforms for this item3d to matrix
    \a m
*/
QMatrix4x4 QQuickItem3DPrivate::localTransforms() const
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
QQuickItem3D::QQuickItem3D(QObject *parent)
    : QQuickItem(0)
{
    d = new QQuickItem3DPrivate(this);

    QQuickItem3D *itemParent = qobject_cast<QQuickItem3D *>(parent);
    if (itemParent) {
        setParentItem(itemParent);
    } else {
        setParent(parent);
    }

    // TODO: Handle QQuickItem3D case
}

/*!
    \internal
*/
QQuickItem3D::~QQuickItem3D()
{
    delete d;
}



/*!
    \qmlproperty vector3D Item3D::position

    The position in 3D space of this item.  The default value for this
    property is (0, 0, 0).

    \sa x, y, z
*/

QVector3D QQuickItem3D::position() const
{
    return d->position;
}

void QQuickItem3D::setPosition(const QVector3D& value)
{
    d->position = value;
    emit position3dChanged();
    update();
}

/*!
    \qmlproperty real Item3D::x

    The x position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, y, z
*/

float QQuickItem3D::x() const
{
    return d->position.x();
}

void QQuickItem3D::setX(float value)
{
    d->position.setX(value);
    emit position3dChanged();
    update();
}

/*!
    \qmlproperty real Item3D::y

    The y position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, x, z
*/

float QQuickItem3D::y() const
{
    return d->position.y();
}

void QQuickItem3D::setY(float value)
{
    d->position.setY(value);
    emit position3dChanged();
    update();
}

/*!
    \qmlproperty real Item3D::z

    The z position of this item in 3D space.  The default value for
    this property is 0.

    \sa position, x, y
*/

float QQuickItem3D::z() const
{
    return d->position.z();
}

void QQuickItem3D::setZ(float value)
{
    d->position.setZ(value);
    emit position3dChanged();
    update();
}

/*!
    \qmlproperty real Item3D::scale

    The scaling factor to apply to the item after the transformations
    from the Item3D::transform property.  The default value for this
    property is 1.

    \sa transform
*/

float QQuickItem3D::scale() const
{
    return d->scale;
}

void QQuickItem3D::setScale(float value)
{
    d->scale = value;
    emit scale3dChanged();
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


QQmlListProperty<QQuickQGraphicsTransform3D> QQuickItem3D::transform()
{
    return QQmlListProperty<QQuickQGraphicsTransform3D>(this, 0, d->transform_append, d->transform_count, d->transform_at, d->transform_clear);
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

QQmlListProperty<QQuickQGraphicsTransform3D> QQuickItem3D::pretransform()
{
    return QQmlListProperty<QQuickQGraphicsTransform3D>(this, 0, d->pretransform_append, d->pretransform_count, d->pretransform_at, d->pretransform_clear);
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
bool QQuickItem3D::inheritEvents() const
{
    return d->inheritEvents;
}

void QQuickItem3D::setInheritEvents(bool inherit)
{
    d->inheritEvents = inherit;

    //Generally we would only want to
    if (inherit)
    {
        for (int index = 0; index < children().size(); ++index) {
            QQuickItem3D *subItem = qobject_cast<QQuickItem3D *>(children().at(index));
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
            QQuickItem3D *subItem = qobject_cast<QQuickItem3D *>(children().at(index));
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
    vertices which when linked as polygons form a recognisable 3D object.  Qt3D currently
    supports a number of these \e {scene formats}, including \e {.obj} file, bezier patches
    \e {(.bez)}, and \e {.3ds} files.

    These meshes are abstracted into the \l Mesh class, which is defined for
    an \l Item3D through this property.

    The default value for this property is null, so a mesh must be defined in
    order for the item to be displayed

    \sa effect
*/

QQuickMesh *QQuickItem3D::mesh() const
{
    return d->mesh;
}

void QQuickItem3D::setMesh(QQuickMesh *value)
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
            connect(value, SIGNAL(animationsChanged()), this, SLOT(updateAnimations()));
            d->requireBlockingEffectsCheck = true;
        }

        emit meshChanged();

        update();
        updateAnimations();
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
QQuickEffect *QQuickItem3D::effect() const
{
    return d->effect;
}

void QQuickItem3D::setEffect(QQuickEffect *value)
{
    if (d->effect == value)
        return;
    if (d->effect)
        disconnect(d->effect, SIGNAL(effectChanged()), this, SLOT(handleEffectChanged()));
    d->effect = value;
    if (d->effect)
    {
        connect(d->effect, SIGNAL(effectChanged()), this, SLOT(handleEffectChanged()));
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

QGLLightParameters *QQuickItem3D::light() const
{
    return d->light;
}

void QQuickItem3D::setLight(QGLLightParameters *value)
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

    The children property contains a list of all QQuickItem derived
    child items for this item.  This provides logical grouping of items in a
    scene. Transformations that are applied to this item will also affect
    child items.  Note that children that are not derived from
    QQuickItem3D will not be rendered at draw time, but will interact
    normally otherwise (e.g. parenting, signal passing etc).  Use a
    qobject_cast if you need to check whether a child is an Item3D.

    The resources property holds all other children that do not
    directly inherit from QQuickItem, such as effects, meshes, and
    other supporting objects.

    Normally it isn't necessary to assign to the children or resources
    properties directly as the QML syntax will take care of the
    assignment depending upon the object's type.

    \sa transform
*/

QQmlListProperty<QObject> QQuickItem3D::resources()
{
    return QQmlListProperty<QObject>(this, 0, QQuickItem3DPrivate::resources_append,
                                     QQuickItem3DPrivate::resources_count,
                                     QQuickItem3DPrivate::resources_at,
                                     QQuickItem3DPrivate::resources_clear);
}



/*!
    \qmlproperty list<Object> Item3D::data

    This property exists to allow future expansion of the Item3D class to
    include additional data and resources.  Currently there is no underlying
    implementation for this.
*/
QQmlListProperty<QObject> QQuickItem3D::data()
{
    return QQmlListProperty<QObject>(this, 0, QQuickItem3DPrivate::data_append, 0, 0, 0);
}

/*!
    \qmlproperty enumeration Item3D::cullFaces

    This property defines the culling method to be use on fragments
    within the item's mesh.  Culling of an item in 3D space can be
    carried out in a number of ways:

    \list
    \li CullDisabled Do not use culling.  This is the default value.
    \li CullFrontFaces Cull the front faces of the object.
    \li CullBackFaces Cull the back faces of the object.
    \li CullAllFaces Cull all faces of the object.
    \li CullClockwise Cull faces based on clockwise winding of vertices.
    \endlist
*/
QQuickItem3D::CullFaces QQuickItem3D::cullFaces() const
{
    return d->cullFaces;
}

void QQuickItem3D::setCullFaces(QQuickItem3D::CullFaces value)
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
    \li DefaultSorting No explicit sorting of the children - draw them in
       whatever order is convenient for the system.  The system may apply
       its own sorting, grouping similar materials to improve performance.
       This is the default.
    \li BackToFront Sort the children to draw them in back-to-front
       order of their \l position, overriding any system-supplied sorting.
       BackToFront is useful when the children are partially transparent
       and must be drawn in back-to-front order for correct rendering.
    \endlist
*/
QQuickItem3D::SortMode QQuickItem3D::sortChildren() const
{
    return d->sortChildren;
}

void QQuickItem3D::setSortChildren(QQuickItem3D::SortMode mode)
{
    if (d->sortChildren != mode) {
        d->sortChildren = mode;
        emit sortChildrenChanged();
    }
}

/*!
    \internal
    Sets the lighting conditions for this item on the \a painter.  The parameters \a currentLight and \a currentLightTransform
    are used to store the current lighting parameters and transforms so that they can be reset on cleanup.

    After drawing has finished the drawLightingCleanup() function should be called to restore previous settings.

    \sa drawLightingCleanup(), draw()
*/
void QQuickItem3D::drawLightingSetup(QGLPainter *painter, const QGLLightParameters *currentLight, QMatrix4x4 &currentLightTransform)
{
    //Lighting
    Q_UNUSED(currentLight)
    if (d->light) {
        currentLight = painter->mainLight();
        currentLightTransform = painter->mainLightTransform();
        painter->setMainLight(d->light);
    }
}

/*!
    \internal
    Restores the lighting conditions for \a painter to the \a currentLight and \a currentLightTransform.  These values are usually
    provided by an earlier call to drawLightingSetup().

    \sa drawLightingSetup(), draw()
*/
void QQuickItem3D::drawLightingCleanup(QGLPainter *painter, const QGLLightParameters *currentLight, QMatrix4x4 &currentLightTransform)
{
    if (d->light)
        painter->setMainLight(currentLight, currentLightTransform);
}

/*!
    \internal
    Sets the effects for this item on the \a painter.  The parameters \a viewportBlend and \a effectBlend
    are used to store the current blending parameters so that they can be reset on cleanup.

    After drawing has finished the drawEffectCleanup() function should be called to restore previous settings.

    \sa drawLightingCleanup(), draw()
*/
void QQuickItem3D::drawEffectSetup(QGLPainter *painter, bool &viewportBlend, bool &effectBlend)
{
    // Blending change for the effect.
    viewportBlend = d->viewport ? d->viewport->blending() : false;
    effectBlend = d->effect ? d->effect->blending() : viewportBlend;
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    //Effects
    if (d->effect)
        d->effect->enableEffect(painter);
}

/*!
    \internal
    Restores the blending settings for \a painter to the \a viewportBlend and \a effectBlend.  These values are usually
    provided by an earlier call to drawEffectSetup().

    \sa drawEffectSetup(), draw()
*/
void QQuickItem3D::drawEffectCleanup(QGLPainter *painter, bool &viewportBlend, bool &effectBlend)
{
    if (d->effect)
        d->effect->disableEffect(painter);
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glDisable(GL_BLEND);
        else
            glEnable(GL_BLEND);
    }
}

/*!
    \internal
    Sets the culling settings for this item.

    After drawing has finished the drawCullCleanup() function should be called to restore previous settings.

    \sa drawCullCleanup(), draw()
*/
void QQuickItem3D::drawCullSetup()
{
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
}

/*!
    \internal
    Restores the culling settings after an earlier call to drawCullSetup().

    \sa drawCullSetup(), draw()
*/
void QQuickItem3D::drawCullCleanup()
{
    if (d->cullFaces != CullDisabled)
        glDisable(GL_CULL_FACE);
}

/*!
    \internal
    Applies the transforms for this item on the \a painter.

    After drawing has finished the drawTransformCleanup() function should be called to restore previous settings.

    \sa drawTransformCleanup(), draw()
*/
void QQuickItem3D::drawTransformSetup(QGLPainter *painter)
{
    //Local and Global transforms
    painter->modelViewMatrix().push();
    painter->modelViewMatrix() *= d->localTransforms();
}

/*!
    \internal
    Restores the previous model-view matrix settings for \a painter after an earlier call to drawTransformSetup().

    \sa drawTransformSetup(), draw()
*/
void QQuickItem3D::drawTransformCleanup(QGLPainter *painter)
{
    //Unset parameters for transforms, effects etc.
    painter->modelViewMatrix().pop();
}

/*!
    \internal
    Iterate through all of the child items for the current item and call their drawing functions.  Children will
    be drawn in the order specified unless specified by sortChildren(), in which case they will be drawn in the
    order specified (usually this will be back-to-front, to allow for transparency of objects).

    \sa sortMode(), draw()
*/
void QQuickItem3D::drawChildren(QGLPainter *painter)
{
    // Find all 3d children for drawing
    QList<QQuickItem3D *> list;;
    foreach (QObject* o, children())
    {
        if (QQuickItem3D *item3d = qobject_cast<QQuickItem3D*>(o))
        {
            list.append(item3d);
        }
    }

    if (d->sortChildren == QQuickItem3D::BackToFront) {
        // Collect up the transformed z positions of all children.
        QList<QPair<float, QQuickItem3D*> > zlist;
        QMatrix4x4 mv = painter->modelViewMatrix();
        for (int index = 0; index < list.size(); ++index) {
            QVector3D position = list.at(index)->position();
            zlist.append(QPair<float, QQuickItem3D*> (mv.map(position).z(), list.at(index)));
        }

        qSort(zlist);
        for (int index = 0; index < zlist.size(); ++index)
            zlist.at(index).second->draw(painter);

    }
    else {
        for (int index = 0; index < list.size(); ++index)
            list.at(index)->draw(painter);
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
    \li 1. Iterate through the child objects of the item and set all lighting parameters found.
    \li 2. Set up culling mode in the painter.
    \li 3. Set effects if they exist.
    \li 4. Set all local model view transformations for this item.
    \li 5. Draw this item.
    \li 6. Iterate through the child objects of the item and draw all child items.
    \li 7. Unset the appropriate parameters and states.
    \endlist



    \sa drawItem(), drawLightingSetup(), drawCullSetup(), drawEffectSetup(), drawChildren(), drawTransformSetup()
*/
void QQuickItem3D::draw(QGLPainter *painter)
{
    // Bail out if this item and its children have been disabled.
    if (!d->isEnabled)
        return;
    if (!d->isInitialized)
        initialize(painter);

    if (!d->bConnectedToOpenGLContextSignal) {
        QQuickWindow * pCanvas = window();
        Q_ASSERT(pCanvas);
        QOpenGLContext* pOpenGLContext = pCanvas->openglContext();
        Q_ASSERT(pOpenGLContext);
        bool Ok = QObject::connect(pOpenGLContext, SIGNAL(aboutToBeDestroyed()), this, SLOT(handleOpenglContextIsAboutToBeDestroyed()), Qt::DirectConnection);
        Q_UNUSED(Ok);  // quell compiler warning
        Q_ASSERT(Ok);
        d->bConnectedToOpenGLContextSignal = true;
    }

    //Setup picking
    int prevId = painter->objectPickId();
    painter->setObjectPickId(d->objectPickId);

    //Setup effect (lighting, culling, effects etc)
    const QGLLightParameters *currentLight = 0;
    QMatrix4x4 currentLightTransform;
    drawLightingSetup(painter, currentLight, currentLightTransform);
    bool viewportBlend, effectBlend;
    drawEffectSetup(painter, viewportBlend, effectBlend);
    drawCullSetup();

    //Local and Global transforms
    drawTransformSetup(painter);

    //Drawing
    drawItem(painter);
    drawChildren(painter);

    //Cleanup
    drawTransformCleanup(painter);
    drawLightingCleanup(painter, currentLight, currentLightTransform);
    drawEffectCleanup(painter, viewportBlend, effectBlend);
    drawCullCleanup();

    //Reset pick id.
    painter->setObjectPickId(prevId);
}

/*!
    \internal
*/
QQuickViewport::~QQuickViewport()
{
}

/*!
    \internal
*/
void QQuickViewport::setItemViewport(QQuickItem3D *item)
{
    item->d->viewport = this;
}

/*!
    \internal
    The process of initialising an /l Object3d is a critical step, particularly in
    complex scenes.  This function initialises the item in \a viewport, and using \a painter.

    During the initialisation process objects are registered as being \e pickable (ie. able
    to be clicked on with the mouse.

    Additionally, in the case of \l Item3D objects which refer to sub-nodes of a mesh, this
    function performs all of the splitting of meshes into sub-branches ready for local
    control by the item.
*/
void QQuickItem3D::initialize(QGLPainter *painter)
{
    if (d->isInitialized) return;

    if (!d->viewport)
    {
        if (QQuickItem3D* parentItem =
                qobject_cast<QQuickItem3D*>(parent()))
        {
            d->viewport = parentItem->d->viewport;
            Q_ASSERT(d->viewport);
        }
    }

    d->objectPickId = d->viewport->registerPickableObject(this);

    for (int index = 0; index < children().size(); ++index) {
        QQuickItem3D *item = qobject_cast<QQuickItem3D *>(children().at(index));
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

/*!
    Returns true if the initialize() has been called, returns false otherwise.

    \sa initialize()
*/
bool QQuickItem3D::isInitialized() const
{
    return d->isInitialized;
}

void QQuickItem3D::componentComplete()
{
    QQuickItem::componentComplete();
    d->componentComplete = true;

    // Now that we have all the mesh and subnode information we need, it's time to setup the mesh scene objects.
    if (mesh() && !meshNode().isEmpty()) {
        int branchNumber = mesh()->createSceneBranch(meshNode());
        if (branchNumber>=0) {
            d->mainBranchId = branchNumber;
        }
        else {
            qWarning()<< "3D item initialization failed: unable to find the specified mesh-node. Defaulting to root node.";
            d->mainBranchId = 0;
        }
    }

    // Find nearest QQuickItem3D parent or Viewport Item
    QQuickItem* parentItem2D = qobject_cast<QQuickItem*>(parentItem());
    // While parent is not null and is not either a QQuickItem3D or the Viewport, loops to find the parent element
    // The dynamic_cast is needed because QQuickViewport is an interface and doesn't have the Q_INTERFACES Macro
    while (parentItem2D && (qobject_cast<QQuickItem3D *>(parentItem2D) == NULL
                            && dynamic_cast<QQuickViewport*>(parentItem2D) == NULL))
        parentItem2D = qobject_cast<QQuickItem*>(parentItem2D->parentItem());
    // If we found a QQuickItem3d or QQuickViewport parent we set it as the direct parent of our component
    if (parentItem2D)
    {
        this->setParentItem(parentItem2D);
        this->setParent(parentItem2D);
    }
    else
    {   // Otherwise if the element has no suitable parent, we insert it in a viewport
        QQuickItem3D *parentItem3D = qobject_cast<QQuickItem3D*>(parentItem());
        if (!parentItem3D && !d->viewport)
        {
            QQmlContext *ctx = QQmlEngine::contextForObject(this);
            QQmlEngine *engine = ctx->engine();
            QQmlComponent vp(engine);
            vp.setData(QByteArray(
                           "import QtQuick 2.0\n"
                           "import Qt3D 2.0\n"
                           "Viewport{ objectName: \"vp\" }\n"), QUrl());
            QObject *implicitViewport = vp.create();
            QQuickItem *parentViewport = qobject_cast<QQuickItem*>(implicitViewport);
            parentViewport->setWidth(width());
            parentViewport->setHeight(height());
            Q_ASSERT(parentViewport);
            Q_ASSERT(parentViewport->objectName() == QLatin1String("vp"));
            QQuickItem *prevParent = parentItem();
            parentViewport->setParentItem(prevParent);
            setParent(parentViewport);
            setParentItem(parentViewport);
        }
    }
    update();
}

/*!
    \internal
    The \c drawItem function performs the actual drawing of the mesh branch which corresponds
    to the section of the mesh being drawn by the \l Item3D to a specific \a painter.
*/
void QQuickItem3D::drawItem(QGLPainter *painter)
{
    if (d->mesh)
    {
        d->mesh->draw(painter, d->mainBranchId);
    }
}

/*!
    \relates QQuickItem3D
    Print a description of \a item to the console.

    If \a detailed is true (which it is by default) then all the properties
    of each node are printed.

    If \a detailed is false, then just one line is printed with the name and
    some identifying information including a unique id for the node.

    The \a indent argument is used internally.
*/
void qDumpItem(QQuickItem3D *item, bool detailed, int indent)
{
    if (item)
    {
        QQuickMesh *mesh = item->mesh();
        QString ind;
        ind.fill(QLatin1Char(' '), indent * 4);
        if (mesh)
        {
            QGLSceneNode *node = mesh->getSceneObject();
            if (node)
                qDumpScene(node, detailed, indent + 1);
            else
                qDebug("%sMesh %p - %s (no node)", qPrintable(ind), mesh, qPrintable(mesh->objectName()));
        }
        else
        {
            qDebug("%sItem %p - %s (no mesh)", qPrintable(ind), item, qPrintable(item->objectName()));
        }
        QObjectList kids = item->children();
        for (int i = 0; i < kids.size(); ++i)
        {
            QQuickItem3D *it = qobject_cast<QQuickItem3D*>(kids.at(i));
            qDumpItem(it, detailed, indent + 1);
        }
    }
}

/*!
    Calculates and returns a matrix that transforms local coordinates into
    world coordinates (i.e. coordinates untransformed by any item3d's
    transforms).
*/
QMatrix4x4 QQuickItem3DPrivate::localToWorldMatrix() const
{
    QMatrix4x4 result = localTransforms();
    QQuickItem3D *anscestor = qobject_cast<QQuickItem3D *>(item->parent());
    while (anscestor)
    {
        result = anscestor->d->localTransforms() * result;
        anscestor = qobject_cast<QQuickItem3D *>(anscestor->parent());
    }
    return result;
}

/*!
    Calculates and returns a matrix that transforms world coordinates into
    coordinates relative to this Item3D.
*/
QMatrix4x4 QQuickItem3DPrivate::worldToLocalMatrix() const
{
    bool inversionSuccessful;
    QMatrix4x4 result = localToWorldMatrix().inverted(&inversionSuccessful);
    if (inversionSuccessful)
        return result;
    qWarning() << "QQuickItem3D - matrix inversion failed trying to generate worldToLocal Matrix";
    return QMatrix4x4();
}

/*!
    Returns the position of a local \a point in world space (i.e. not
    transformed by this item, it's parents, the camera etc).
*/
QVector3D QQuickItem3D::localToWorld(const QVector3D &point) const
{
    return d->localToWorldMatrix() * point;
}

/*!
    Returns the position of a point in world space in local coordinates.
*/
QVector3D QQuickItem3D::worldToLocal(const QVector3D &point) const
{
    return d->worldToLocalMatrix() * point;
}

/*!
    \internal
    This function handles the standard mouse events for the item as contained in \a e.

    Returns the boolean value of the regular QObject::event() function.oo
*/

bool QQuickItem3D::event(QEvent *e)
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

void QQuickItem3D::handleEffectChanged()
{
    d->requireBlockingEffectsCheck = true;
    update();
}

void QQuickItem3D::handleOpenglContextIsAboutToBeDestroyed()
{
    if (d->mesh) {
        d->mesh->openglContextIsAboutToBeDestroyed();
    }
    if (d->effect) {
        d->effect->openglContextIsAboutToBeDestroyed();
    }
}

/*!
    \qmlproperty string Item3D::meshNode

    This property is a simple string which refers to the node in the \l Mesh object which
    is associated with this \l Item3D.

    \sa mesh
*/
QString QQuickItem3D::meshNode() const
{
    return d->meshNode;
}

void QQuickItem3D::setMeshNode(const QString &node)
{
    //the actual instantiation of the node as the mesh itself is undertaken in the initialize function.
    d->meshNode = node;
}

/*!
    \internal
    Update the \l Viewport with which this item is associated.
*/
void QQuickItem3D::update()
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
            }

            if (this->effect())
            {
                for (int i = 0; i < k.size(); ++i)
                {
                    this->effect()->applyTo(k.at(i));
                }
            }

        }
        d->requireBlockingEffectsCheck = false;
    }
    if (d->viewport)
        d->viewport->update3d();
}

/*!
    \internal
    Update the animations property.
    //TODO
*/
void QQuickItem3D::updateAnimations()
{
    d->animations.clear();
    if (d->mesh) {
        const QList<QGLSceneAnimation *>& rSrcList = d->mesh->getAnimations();
        for (int i=0; i<rSrcList.count(); ++i) {
            d->animations.append(new QQuickAnimation3D(rSrcList.at(i),0));
        }
    }
    emit animationsChanged();
}

/*!
    \qmlproperty bool Item3D::enabled

    This property should be set to true to enable the drawing
    of this item and its children.  Set this property to false
    to disable drawing.  The default value is true.

    \sa mesh
*/
bool QQuickItem3D::isEnabled() const
{
    return d->isEnabled;
}

void QQuickItem3D::setEnabled(bool value)
{
    if (d->isEnabled != value) {
        d->isEnabled = value;
        emit enabledChanged();
    }
}

/*!
    //TODO
*/
QQmlListProperty<QQuickAnimation3D> QQuickItem3D::animations()
{
    return QQmlListProperty<QQuickAnimation3D>(this, 0, d->animations_append, d->animations_count, d->animations_at, d->animations_clear);
}

/*!
  Returns the unique pick ID for this item.
*/
int QQuickItem3D::objectPickId() const
{
    return d->objectPickId;
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
