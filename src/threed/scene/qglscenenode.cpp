/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglscenenode_p.h"
#include "qglpicknode.h"
#include "qglpainter.h"
#include "qgeometrydata.h"
#include "qglmaterialcollection.h"
#include "qglrendersequencer.h"
#include "qglabstracteffect.h"
#include "qgraphicstransform3d.h"

#ifndef QT_NO_DEBUG_STREAM
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#endif

#include <QtGui/qmatrix4x4.h>
#if !defined(QT_NO_THREAD)
#include <QtCore/qthread.h>
#include <QtGui/qapplication.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneNode
    \brief The QGLSceneNode class defines a node in a 3D scene.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    QGLSceneNode represents one component of a scene.  The QGLSceneNode
    class manages materials, effects and transformations for itself and
    for its child nodes.

    As a general approach to 3D applications, a tree of QGLSceneNodes may
    be constructed, geometry added to them, materials and transformations
    applied, all during application initialization; and then by simply
    calling the draw() function the scene is easily rendered for each frame.

    \section1 Geometry

    Multiple QGLSceneNodes can reference the same geometry, whilst
    applying different transformations and treatments to it.  Since
    QGLSceneNode is a QObject sub class it cannot be copied directly, so
    instead use the clone() function for this purpose.

    A scene node allows referencing into sub-parts of geometry, via the start
    and count properties.

    The start index is an offset into the geometry at which drawing will start.
    The default start index is 0, so that drawing will start from the beginning
    of the geometry.  The count dictates how many vertices will be drawn.  The
    default count is 0, which instructs the underlying geometry to draw all
    vertices.

    A node may have no geometry, that is \c{geometry().count() == 0}.  This is
    useful for example to have one node controlling or collecting together
    several child nodes to be manipulated as a unit.

    \section1 Materials

    Also a node may have a local material.  This allows drawing the same geometry
    with different materials (which includes different textures).

    When accessing a QGLSceneNode via QML, or for simple applications, the
    pointer based material functions are convenient and intuitive, saving the
    trouble of adding the material pointer to the correct palette:
    \list
        \i material()
        \i setMaterial()
        \i backMaterial()
        \i setBackMaterial()
    \endlist

    For more complex applications; for example building model loaders, or for
    larger scenes; where you need to explicitly manage materials via a palette,
    use the index based functions:
    \list
        \i materialIndex()
        \i setMaterialIndex()
        \i backMaterialIndex()
        \i setBackMaterialIndex()
    \endlist

    The behaviour of both with respect to faces is the same - if a material()
    is specified but no backMaterial() is specified, then the material() is
    applied to both faces; if both material() and backMaterial() are non-null
    then they are applied to their specific faces.

    \section1 Transformations

    Typically the local transformation matrix is set by the process that
    constructed the node:  in the case of an imported model, it is likely
    to have been specified by the model file.  To make individual changes
    to the location or orientation of this node, use the position() and
    transforms() properties.

    \section1 Scene Graph

    Use childNodes() to obtain the list of child nodes, and add and remove
    child nodes by the addNode() and removeNode() methods.  If a QGLSceneNode
    is constructed with a QGLSceneNode parent, then addNode() will be
    called implicitly on the parent.

    A child may be a child multiple times, a child may be under more than one
    parent, and several parents may reference the same child.  There is however
    no protection against cycles, so a child must not be a parent of itself,
    even if indirectly.

    A child node for the purposes of rendering means a child added via the
    addNode() method.  The default QGLSceneNode constructor will check to
    see if its parent is a QGLSceneNode and add itself via the addNode()
    function if it is.

    To help debug a scene, use the qDumpScene() function to get a printout
    on stderr of the entire structure of the scene below the argument node.

    \section1 Debugging Lighting Normals

    The ViewNormals option is an advanced feature for use when inspecting
    and debugging models or geometry in a scene.  The lighting normals
    are displayed as a straight line from the vertex pointing in
    the direction of the lighting normal.  This is useful for
    example to show where normals are inverted or wrongly
    calculated.

    The setting of the ViewNormals flag is not propagated to child nodes,
    instead set the flag to true for the node or nodes where its
    needed.  To set the flag on all child nodes use code like:

    \code
    foreach (QGLSceneNode *node, scene.allChildren())
        node->setNormalViewEnabled(true);
    \endcode

    \image spiky-teapot.png

    \sa QGLAbstractScene
*/

/*!
    \enum QGLSceneNode::Option
    This enum defines option flags for QGLSceneNode.  By default the none of the flags
    are set, so the options() function returns QGLSceneNode::NoOptions

    \value NoOptions Do not enable any QGLSceneNode options.
    \value CullBoundingBox Perform a cull using boundingBox() before
        attempting to draw the geometry().
    \value ViewNormals Enables the display of lighting normals for
        debugging purposes.
    \value ReportCulling Send a signal when an object is displayed or culled.
    \value HideNode Hide this node so it, and all its children, are excluded from rendering.
    \sa setOptions()
*/

/*!
    Constructs a new scene node and attaches it to \a parent.  If parent is
    a QGLSceneNode then this node is added to it as a child.
*/
QGLSceneNode::QGLSceneNode(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGLSceneNodePrivate())
{
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}

/*!
    \internal
    Used by clone().
*/
QGLSceneNode::QGLSceneNode(QGLSceneNodePrivate *d, QObject *parent)
    : QObject(parent)
    , d_ptr(d)
{
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}

/*!
    Constructs a new scene node referencing \a geometry and attaches it to
    \a parent.    If parent is a QGLSceneNode then this node is added to it
    as a child.
*/
QGLSceneNode::QGLSceneNode(const QGeometryData &geometry, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGLSceneNodePrivate())
{
    Q_D(QGLSceneNode);
    d->geometry = geometry;
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}

/*!
    Destroys this scene node.
*/
QGLSceneNode::~QGLSceneNode()
{
    Q_D(QGLSceneNode);

    // Detach ourselves from our children.  The children will be
    // deleted separately when their QObject::parent() deletes them.
    for (int index = 0; index < d->childNodes.count(); ++index)
        d->childNodes.at(index)->d_ptr->parentNodes.removeOne(this);

    // Detach ourselves from our remaining parents, and notify them
    // to update their bounding boxes.  This won't be needed if we
    // are recursively destroying a tree of nodes because the parent
    // already detached from this node above.
    for (int index = 0; index < d->parentNodes.count(); ++index) {
        QGLSceneNode *parent = d->parentNodes.at(index);
        parent->d_ptr->childNodes.removeOne(this);
        parent->invalidateBoundingBox();
    }
}

/*!
    \property QGLSceneNode::options
    \brief Various option flags effecting rendering

    This property containst several flags that modify normal rendering and
    culling on a QGLSceneNode.  See \l Options for a full description of the
    available flags.

    \sa setOption(), Options
*/

/*!
    Returns the drawing options associated with this node.
    The default is 0 (no options set).

    \sa setOptions(), setOption()
*/
QGLSceneNode::Options QGLSceneNode::options() const
{
    Q_D(const QGLSceneNode);
    return d->options;
}

/*!
    \qmlproperty enumeration QGLSceneNode::options

    Defines the settings of various options configurabled on
    nodes in the mesh.

    \list
    \o NoOptions Use no options.  This is the default.
    \o CullBoundingBox Use the camera position to cull the whole node if possible.
    \o ViewNormals Turn on normals debugging mode visually depict lighting normals.
    \o ReportCulling Send a signal when an object is displayed or culled.
    \endlist
*/

/*!
    Sets the drawing \a options associated with this node.

    \sa options(), setOption()
*/
void QGLSceneNode::setOptions(QGLSceneNode::Options options)
{
    Q_D(QGLSceneNode);
    if (d->options != options) {
        d->options = options;
        emit updated();
    }
}

/*!
    Enables or disables \a option according to \a value.

    \sa options(), setOptions()
*/
void QGLSceneNode::setOption(QGLSceneNode::Option option, bool value)
{
    Q_D(QGLSceneNode);
    QGLSceneNode::Options opts = d->options;
    if (value)
        opts |= option;
    else
        opts &= ~option;
    if (d->options != opts) {
        d->options = opts;
        emit updated();
    }
}

/*!
    Returns the geometry associated with this node, or a null QGeometryData
    if no geometry has been associated with it.

    \sa setGeometry()
*/
QGeometryData QGLSceneNode::geometry() const
{
    Q_D(const QGLSceneNode);
    return d->geometry;
}

/*!
    Sets the geometry associated with this node to be \a geometry.
    Typically the \a geometry will be some type of mesh object.  The
    default implementation of the QGLSceneNode::draw() method will call
    the geometry's draw() method.

    \sa geometry()
*/
void QGLSceneNode::setGeometry(QGeometryData geometry)
{
    Q_D(QGLSceneNode);
    d->geometry = geometry;
    emit updated();
}

/*!
    Returns a bounding box for the portion of the geometry referenced by
    this scene node.  If the value of start() is 0, and count() is the same
    as geometry()->size() then the bounding box will be the same as
    geometry()->boundingBox().  However if the scene node only references
    some part of the geometry, a bounding box for this section is calculated.

    If this scene node has child nodes then the bounding box will be the
    calculated union of the bounding box for this nodes geometry (if any) and
    the bounding boxes of the children.

    The calculated value is cached and returned on subsequent calls, but
    could be expensive to calculate initially.
*/
QBox3D QGLSceneNode::boundingBox() const
{
    Q_D(const QGLSceneNode);
    if (d->boxValid)
        return d->bb;
    d->bb = QBox3D();
    if (d->geometry.count() > 0)
    {
        if (d->start == 0 && (d->count == d->geometry.count() || d->count == 0))
        {
            d->bb = d->geometry.boundingBox();
        }
        else
        {
            QGL::IndexArray indices = d->geometry.indices();
            for (int i = d->start; i < (d->start + d->count); ++i)
            {
                int ix = indices.at(i);
                d->bb.unite(d->geometry.vertexAt(ix));
            }
        }
    }
    QList<QGLSceneNode*>::const_iterator it = d->childNodes.constBegin();
    for ( ; it != d->childNodes.constEnd(); ++it)
    {
        QGLSceneNode *n = *it;
        QBox3D b = n->boundingBox();
        d->bb.unite(b);
    }
    d->bb.transform(transform());
    d->boxValid = true;
    return d->bb;
}

// Calculate the resulting matrix from the position, local transform,
// and list of transforms.
QMatrix4x4 QGLSceneNode::transform() const
{
    Q_D(const QGLSceneNode);
    QMatrix4x4 m;
    if (!d->translate.isNull())
        m.translate(d->translate);
    if (!d->localTransform.isIdentity())
        m *= d->localTransform;
    for (int index = d->transforms.size() - 1; index >= 0; --index)
        d->transforms.at(index)->applyTo(&m);
    return m;
}

/*!
    Returns the local transform associated with this node.  If no
    local transform has been explicitly set, this method returns a
    QMatrix4x4 set to the identity matrix.

    The local transform is typically set during model loading or
    geometry construction, and is a feature of the geometry.

    In general to change the location or orientation of the node
    use the position() or transforms() properties instead.

    \sa setLocalTransform(), position(), transforms()
*/
QMatrix4x4 QGLSceneNode::localTransform() const
{
    Q_D(const QGLSceneNode);
    return d->localTransform;
}

/*!
    Sets the local transform associated with this node to be \a transform.
    The default implementation of the QGLSceneNode::draw() method will
    apply this transform to the QGLPainter before drawing any geometry.

    \sa localTransform()
*/
void QGLSceneNode::setLocalTransform(const QMatrix4x4 &transform)
{
    Q_D(QGLSceneNode);
    if (d->localTransform != transform)
    {
        d->localTransform = transform;
        emit updated();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::position
    \brief The amounts of x, y and z axis translation for this node.

    Since most nodes are situated relative to \c{(0, 0, 0)} when imported as
    part of a model or constructed programatically, the translation is
    effectively the position of the model in the scene.

    The x, y and z axis translations can also be specified individually as
    separate properties \l x, \l y, and \l z

    \sa x(), y(), z()
*/
QVector3D QGLSceneNode::position() const
{
    Q_D(const QGLSceneNode);
    return d->translate;
}

void QGLSceneNode::setPosition(const QVector3D &p)
{
    Q_D(QGLSceneNode);
    if (p != d->translate)
    {
        d->translate = p;
        emit updated();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::x
    \brief The amount of x axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::x() const
{
    Q_D(const QGLSceneNode);
    return d->translate.x();
}

void QGLSceneNode::setX(qreal x)
{
    Q_D(QGLSceneNode);
    if (x != d->translate.x())
    {
        d->translate.setX(x);
        emit updated();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::y
    \brief The amount of y axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::y() const
{
    Q_D(const QGLSceneNode);
    return d->translate.y();
}

void QGLSceneNode::setY(qreal y)
{
    Q_D(QGLSceneNode);
    if (y != d->translate.y())
    {
        d->translate.setY(y);
        emit updated();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::z
    \brief The amount of z axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::z() const
{
    Q_D(const QGLSceneNode);
    return d->translate.z();
}

void QGLSceneNode::setZ(qreal z)
{
    Q_D(QGLSceneNode);
    if (z != d->translate.z())
    {
        d->translate.setZ(z);
        emit updated();
        invalidateTransform();
    }
}

/*!
    Returns the list of transformations to apply to this node.
    The default is the empty list.

    The transformations are applied to the node itself, so a
    QGraphicsScale3D followed by a QGraphicsTranslation3D will
    first scale the node in its local co-ordinate system,
    and then translate the node a new location.

    In the mathematical sense, the transformations are applied to
    the modelview matrix in the reverse order in which they appear
    in this list.

    The position() is applied after all other transformations
    have been applied.

    \sa setTransforms(), addTransform(), position()
*/
QList<QGraphicsTransform3D *> QGLSceneNode::transforms() const
{
    Q_D(const QGLSceneNode);
    return d->transforms;
}

/*!
    Sets the list of transformations to apply to this node to \a transforms.

    The transformations are applied to the node itself, so a
    QGraphicsScale3D followed by a QGraphicsTranslation3D will
    first scale the node in its local co-ordinate system,
    and then translate the node a new location.

    In the mathematical sense, the transformations are applied to
    the modelview matrix in the reverse order in which they appear
    in \a transforms.

    The position() is applied after all other transformations
    have been applied.

    \sa transforms(), addTransform(), position()
*/
void QGLSceneNode::setTransforms(const QList<QGraphicsTransform3D *> &transforms)
{
    Q_D(QGLSceneNode);
    for (int index = 0; index < d->transforms.size(); ++index) {
        QGraphicsTransform3D *transform = d->transforms.at(index);
        disconnect(transform, SIGNAL(transformChanged()), this, SLOT(transformChanged()));
    }
    d->transforms.clear();
    for (int index = 0; index < transforms.size(); ++index) {
        QGraphicsTransform3D *transform = transforms.at(index);
        if (transform) {
            connect(transform, SIGNAL(transformChanged()), this, SLOT(transformChanged()));
            d->transforms.append(transform);
        }
    }
    emit updated();
    invalidateTransform();
}

/*!
    Adds a single \a transform to this node, to be applied to the
    node after all current members of transformations() have been applied.

    In the mathematical sense, \a transform is applied to the modelview
    matrix before the current members of transformations() are applied
    in reverse order.

    \sa transforms(), setTransforms()
*/
void QGLSceneNode::addTransform(QGraphicsTransform3D *transform)
{
    Q_D(QGLSceneNode);
    if (!transform)
        return;     // Avoid nulls getting into the transform list.
    connect(transform, SIGNAL(transformChanged()), this, SLOT(transformChanged()));
    d->transforms.append(transform);
    emit updated();
    invalidateTransform();
}

/*!
    \internal
*/
void QGLSceneNode::transformChanged()
{
    invalidateTransform();
    emit updated();
}

/*!
    Returns the drawing mode to use to render geometry().  The default
    is QGL::Triangles.

    \sa setDrawingMode()
*/
QGL::DrawingMode QGLSceneNode::drawingMode() const
{
    Q_D(const QGLSceneNode);
    return d->drawingMode;
}

/*!
    Sets the drawing \a mode to use to render geometry().

    Note: this function changes the drawing mode, but the underlying
    geometry() still consists of the triangles that were added.
    Thus, this function is only useful for converting the drawing mode
    into QGL::Points to display the geometry() as a point cloud
    instead of a triangle mesh.  The other enums from QGL::DrawingMode
    will give unpredictable results.

    \sa drawingMode()
*/
void QGLSceneNode::setDrawingMode(QGL::DrawingMode mode)
{
    Q_D(QGLSceneNode);
    if (d->drawingMode != mode)
    {
        d->drawingMode = mode;
        emit updated();
    }
}

/*!
    Returns the drawing width for this node.

    Drawing width is used only when drawing lines or points (ie. when
    the drawing mode is points, lines, line-strips, etc).

    \sa drawingMode()
*/
qreal QGLSceneNode::drawingWidth() const
{
    Q_D(const QGLSceneNode);
    return d->drawingWidth;
}

/*!
    Sets the drawing \a width to the given value.

    Drawing width is used only when drawing lines or points (ie. when
    the drawing mode is points, lines, line-strips, etc).

    \sa drawingMode()
*/
void QGLSceneNode::setDrawingWidth(qreal width)
{
    Q_D(QGLSceneNode);
    d->drawingWidth = width;
}


/*!
    Returns the local effect associated with this node.  The default value
    is QGL::FlatColor.  If the value of hasEffect() is false, then this
    the value of effect() is ignored.

    \sa setEffect(), hasEffect()
*/
QGL::StandardEffect QGLSceneNode::effect() const
{
    Q_D(const QGLSceneNode);
    return d->localEffect;
}

/*!
    Sets the local effect associated with this node to be \a effect.  hasEffect()
    will return true after calling this method.

    The QGLSceneNode::draw() function will ensure that \a effect is applied to the
    QGLPainter before drawing any geometry.

    \sa effect(), hasEffect()
*/
void QGLSceneNode::setEffect(QGL::StandardEffect effect)
{
    Q_D(QGLSceneNode);
    if (d->localEffect != effect || !d->hasEffect) {
        d->localEffect = effect;
        d->hasEffect = true;
        emit updated();
    }
}

/*!
    Returns the user effect associated with this node, or NULL if one is not
    set.  The default value is NULL.  If the value of hasEffect() is false,
    then this effect is ignored.

    \sa setUserEffect(), hasEffect()
*/
QGLAbstractEffect *QGLSceneNode::userEffect() const
{
    Q_D(const QGLSceneNode);
    return d->customEffect;
}

/*!
    Sets the local effect associated with this node to be the custom
    \a effect.  hasEffect() will return true after calling this method.

    This custom effect will supersede any standard effect.

    The default implementation of QGLSceneNode::apply() will set this effect
    during initialization of the model.

    The default implementation of the QGLSceneNode::draw() method will
    ensure that \a effect is applied to the QGLPainter before drawing
    any geometry.

    \sa userEffect(), hasEffect()
*/
void QGLSceneNode::setUserEffect(QGLAbstractEffect *effect)
{
    Q_D(QGLSceneNode);
    if (d->customEffect != effect || !d->hasEffect) {
        d->customEffect = effect;
        d->hasEffect = true;
        emit updated();
    }
}


 /*!
     Returns true if the local effect on this node is enabled, otherwise
     returns false.

     \sa setEffectEnabled(), setEffect()
 */
bool QGLSceneNode::hasEffect() const
{
    Q_D(const QGLSceneNode);
    return d->hasEffect;
}

/*!
    Sets whether the current value of effect() or userEffect() will be
    applied to the QGLPainter prior to drawing.  If \a enabled is true,
    then the effect is applied, otherwise it is not.

     \sa setEffect(), effect(), hasEffect()
*/
void QGLSceneNode::setEffectEnabled(bool enabled)
{
    Q_D(QGLSceneNode);
    if (d->hasEffect != enabled) {
        d->hasEffect = enabled;
        emit updated();
    }
}

/*!
    Returns the starting index within geometry() that should be used
    to render fragments for this scene node.  The default value is 0,
    indicating that the 0'th logical vertex in geometry() is the start.

    \sa setStart(), count()
*/
int QGLSceneNode::start() const
{
    Q_D(const QGLSceneNode);
    return d->start;
}

/*!
    Sets the \a start index within geometry() that should be used
    to render fragments for this scene node.

    \sa start(), setCount()
*/
void QGLSceneNode::setStart(int start)
{
    Q_D(QGLSceneNode);
    if (start != d->start)
    {
        d->start = start;
        emit updated();
        invalidateBoundingBox();
    }
}

/*!
    Returns the count of the vertices to render from geometry()
    for this scene node.  The default is zero, meaning that this node
    uses all vertices from start() up to the last logical vertex
    in the underlying geometry().

    \sa setCount(), start()
*/
int QGLSceneNode::count() const
{
    Q_D(const QGLSceneNode);
    return d->count;
}

/*!
    Sets the \a count of the vertices to render from geometry()
    for this scene node.

    \sa count(), setStart()
*/
void QGLSceneNode::setCount(int count)
{
    Q_D(QGLSceneNode);
    if (count != d->count)
    {
        d->count = count;
        emit updated();
        invalidateBoundingBox();
    }
}

/*!
    Returns the material index for this scene node.

    \sa setMaterialIndex()
*/
int QGLSceneNode::materialIndex() const
{
    Q_D(const QGLSceneNode);
    return d->material;
}

/*!
    Sets the material index for this scene node to \a material.

    \sa materialIndex()
*/
void QGLSceneNode::setMaterialIndex(int material)
{
    Q_D(QGLSceneNode);
    if (d->material != material) {
        d->material = material;
        emit updated();
    }
}

/*!
    Returns the back material index for this scene node.

    \sa setBackMaterialIndex()
*/
int QGLSceneNode::backMaterialIndex() const
{
    Q_D(const QGLSceneNode);
    return d->backMaterial;
}

/*!
    Sets the back material index for this scene node to \a material.

    \sa materialIndex()
*/
void QGLSceneNode::setBackMaterialIndex(int material)
{
    Q_D(QGLSceneNode);
    if (d->backMaterial != material) {
        d->backMaterial = material;
        emit updated();
    }
}

/*!
    \property QGLSceneNode::material
    \brief This property is a pointer to a QGLMaterial instance for this node.

    This material is applied to all faces if the backMaterial() property
    is set to null, which is the default.  If the backMaterial() property is non-null
    then this material is only applied to the front faces.

    To apply a material to the back faces use the backMaterial() property.

    Getting this property is exactly equivalent to
    \c{palette()->material(materialIndex())}.

    Setting this property causes the material if not already in this nodes palette to be
    added, and then the corresponding index to be set for this scene node.

    Setting this property is equivalent to:
    \code
    int index = d->palette->indexOf(material);
    if (index == -1)
        index = d->palette->addMaterial(material);
    setMaterialIndex(index);
    \endcode

    If setting this property, when no palette exists one is created, as a
    convenience - but this is suitable only for experimental code and for
    \bold{very small numbers of nodes}.  In debug mode a warning is
    printed in this case.

    Generally one common palette should be created, and set on each node.  This
    also allows nodes to share materials and their textures.

    \sa materialIndex(), setMaterialIndex()
*/
QGLMaterial *QGLSceneNode::material() const
{
    Q_D(const QGLSceneNode);
    if (d->palette)
        return d->palette->material(d->material);
    return 0;
}

void QGLSceneNode::setMaterial(QGLMaterial *material)
{
    Q_D(QGLSceneNode);
    if (!d->palette)
        d->palette = new QGLMaterialCollection(this);
    int ix = d->palette->indexOf(material);
    if (ix == -1)
        ix = d->palette->addMaterial(material);
    setMaterialIndex(ix);
}

/*!
    \property QGLSceneNode::backMaterial
    \brief This property is a pointer to any QGLMaterial instance for this node's back faces.

    This material is applied to the back faces, if non-null.  The default value
    of this property is null.  When this property is null, any non-null material
    set on the material() property will be applied to front and back faces.

    To apply a material to the front faces use the material() property.

    Getting this property is exactly equivalent to
    \c{palette()->material(backMaterialIndex())}.

    Setting this property causes the material if not already in this nodes palette to be
    added, and then the corresponding index to be set for this scene node.

    Setting this property is exactly equivalent to:
    \code
    int index = d->palette->indexOf(material);
    if (index == -1)
        index = d->palette->addMaterial(material);
    setBackMaterialIndex(index);
    \endcode
*/
QGLMaterial *QGLSceneNode::backMaterial() const
{
    Q_D(const QGLSceneNode);
    if (d->palette)
        return d->palette->material(d->backMaterial);
    return 0;
}

void QGLSceneNode::setBackMaterial(QGLMaterial *material)
{
    Q_D(QGLSceneNode);
    if (!d->palette)
        d->palette = new QGLMaterialCollection(this);
    int ix = d->palette->indexOf(material);
    if (ix == -1)
        ix = d->palette->addMaterial(material);
    setBackMaterialIndex(ix);
}
/*!
    Returns the palette of materials used by this scene node, or NULL
    if no palette has been set.

    \sa setPalette()
*/
QGLMaterialCollection *QGLSceneNode::palette() const
{
    Q_D(const QGLSceneNode);
    return d->palette;
}

/*!
    Sets the palette of materials for this scene node to \a palette.

    \sa palette()
*/
void QGLSceneNode::setPalette(QGLMaterialCollection *palette)
{
    Q_D(QGLSceneNode);
    if (d->palette != palette) {
        d->palette = palette;
        emit updated();
    }
}

/*!
    Returns a list of the child nodes for this node.  This list is not
    recursively generated, it includes only the nodes which are
    immediate children of this node.

    \sa allChildren()
*/
QList<QGLSceneNode*> QGLSceneNode::children() const
{
    Q_D(const QGLSceneNode);
    return d->childNodes;
}

/*!
    Returns a list including recursively all child nodes under
    this node.  Each child node only appears once, even if it is included
    multiple times in the scene graph.

    \sa children()
*/
QList<QGLSceneNode*> QGLSceneNode::allChildren() const
{
    Q_D(const QGLSceneNode);
    QList<QGLSceneNode*> allSceneNodes;
    QList<QGLSceneNode*> gather;
    QList<QGLSceneNode*>::const_iterator it = d->childNodes.constBegin();
    for ( ; it != d->childNodes.constEnd(); ++it)
        if (!gather.contains(*it))
            gather.append(*it);
    while (gather.count() > 0)
    {
        QGLSceneNode *node = gather.takeFirst();
        if (!allSceneNodes.contains(node))
        {
            allSceneNodes.append(node);
            gather.append(node->children());
        }
    }
    return allSceneNodes;
}

/*!
    Adds the \a node to the list of child nodes for this node.

    This function does nothing if \a node is null or is already a child
    of this node.  If the \a node is the node itself, a warning about an
    attempt to add a node to itself is printed, and no add takes place.

    If the aim is to have the same geometry displayed several times under a
    given node, each time with different transformations, use the clone()
    call to create copies of the node and then apply the transformations to
    the copies.

    Alternatively, create modifier nodes with the transformations and add the
    geometry bearing node to each with addNode():
    \code
    QGLBuilder builder;
    builder << CarWheel(5.0f); // some car wheel geometry
    QGLSceneNode wheel = builder.finalizedSceneNode();
    QGLSceneNode frontLeft = new QGLSceneNode(m_sceneRoot);
    frontLeft->addNode(wheel);
    frontLeft->setPosition(QVector3D(1.0f, 2.0f, 0.0f));
    QGLSceneNode frontRight = new QGLSceneNode(m_sceneRoot);
    frontRight->addNode(wheel);
    frontRight->setPosition(QVector3D(-1.0f, 2.0f, 0.0f));
    QGLSceneNode backLeft = new QGLSceneNode(m_sceneRoot);
    backLeft->addNode(wheel);
    backLeft->setPosition(QVector3D(1.0f, -2.0f, 0.0f));
    QGLSceneNode backRight = new QGLSceneNode(m_sceneRoot);
    backRight->addNode(wheel);
    backRight->setPosition(QVector3D(-1.0f, -2.0f, 0.0f));
    \endcode

    Because a child node can be validly added to many different nodes,
    calling addNode() does not normally affect the QObject::parent()
    ownership.  However, if \a node does not currently have a
    QObject::parent(), the parent will be set to this node.

    \sa removeNode(), clone(), addNodes()
*/
void QGLSceneNode::addNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    bool alreadyAdded = node && node->d_ptr->parentNodes.contains(this);
    if (!node || node == this || alreadyAdded)
        return;     // Invalid node, or already under this parent.
    invalidateBoundingBox();
    d->childNodes.append(node);
    node->d_ptr->parentNodes.append(this);
    if (!node->parent())
        node->setParent(this);
    connect(node, SIGNAL(updated()), this, SIGNAL(updated()));
    emit updated();
}

/*!
    Adds the members of \a nodes to the list of child nodes
    for this node.

    \sa addNode(), removeNodes()
*/
void QGLSceneNode::addNodes(const QList<QGLSceneNode *> &nodes)
{
    Q_D(QGLSceneNode);
    for (int index = 0; index < nodes.count(); ++index) {
        QGLSceneNode *node = nodes.at(index);
        if (!node || node->d_ptr->parentNodes.contains(this))
            continue;   // Invalid node, or already under this parent.
        d->childNodes.append(node);
        node->d_ptr->parentNodes.append(this);
        if (!node->parent())
            node->setParent(this);
        connect(node, SIGNAL(updated()), this, SIGNAL(updated()));
    }
    invalidateBoundingBox();
    emit updated();
}

/*!
    Removes the child node matching \a node from this node.

    If the QObject::parent() ownership of \a node was set to this
    node, then its parent will be changed to another parent node if it
    had multiple parents.

    If \a node had only a single parent, then its parent will be set to null,
    effectively detaching it from the QObject ownership rules of the scene
    graph.  The caller is then responsible for deleting \a node.

    If the QObject::parent() of \a node was not a scene node parent,
    then it will be left unmodified.

    \sa addNode(), removeNodes()
*/
void QGLSceneNode::removeNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    if (!node || !node->d_ptr->parentNodes.contains(this))
        return;     // Invalid node or not attached to this parent.
    d->childNodes.removeOne(node);
    node->d_ptr->parentNodes.removeOne(this);
    if (node->parent() == this) {
        // Transfer QObject ownership to another parent, or null.
        if (!node->d_ptr->parentNodes.isEmpty())
            node->setParent(node->d_ptr->parentNodes[0]);
        else
            node->setParent(0);
    }
    disconnect(node, SIGNAL(updated()), this, SIGNAL(updated()));
    invalidateBoundingBox();
    emit updated();
}

/*!
    Removes the members of \a nodes from the list of child nodes
    for this node.

    \sa removeNode(), addNodes()
*/
void QGLSceneNode::removeNodes(const QList<QGLSceneNode *> &nodes)
{
    Q_D(QGLSceneNode);
    for (int index = 0; index < nodes.count(); ++index) {
        QGLSceneNode *node = nodes.at(index);
        if (!node || !node->d_ptr->parentNodes.contains(this))
            continue;   // Invalid node or not attached to this parent.
        d->childNodes.removeOne(node);
        node->d_ptr->parentNodes.removeOne(this);
        if (node->parent() == this) {
            // Transfer QObject ownership to another parent, or null.
            if (!node->d_ptr->parentNodes.isEmpty())
                node->setParent(node->d_ptr->parentNodes[0]);
            else
                node->setParent(0);
        }
        disconnect(node, SIGNAL(updated()), this, SIGNAL(updated()));
    }
    invalidateBoundingBox();
    emit updated();
}

void QGLSceneNode::invalidateBoundingBox() const
{
    Q_D(const QGLSceneNode);
    d->boxValid = false;
    d->invalidateParentBoundingBox();
}

void QGLSceneNode::invalidateTransform() const
{
    invalidateBoundingBox();
}

void QGLSceneNode::drawNormalIndicators(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    QVector3DArray verts;
    QGL::IndexArray indices = d->geometry.indices();
    for (int i = d->start; i < (d->start + d->count); ++i)
    {
        int ix = indices[i];
        QVector3D a = d->geometry.vertexAt(ix);
        QVector3D b = a + d->geometry.normalAt(ix);
        verts.append(a, b);
    }
    painter->setVertexAttribute(QGL::Position, QGLAttributeValue(verts));
    glLineWidth(2.0f);
    painter->draw(QGL::Lines, verts.size());
}

const QGLMaterial *QGLSceneNode::setPainterMaterial(int material, QGLPainter *painter,
                                              QGL::Face faces, bool &changedTex)
{
    Q_D(QGLSceneNode);
    QGLMaterial *mat = d->palette->material(material);
    const QGLMaterial *saveMat = 0;
    if (painter->faceMaterial(faces) != mat)
    {
        saveMat = painter->faceMaterial(faces);
        painter->setFaceMaterial(faces, mat);
        int texUnit = 0;
        for (int i = 0; i < mat->textureLayerCount(); ++i)
        {
            QGLTexture2D *tex = mat->texture(i);
            if (tex)
            {
                painter->glActiveTexture(GL_TEXTURE0 + texUnit);
                tex->bind();
                changedTex = true;
                ++texUnit;
            }
        }
    }
    return saveMat;
}

/*!
    Draws the geometry of the node onto the \a painter.

    This is the function which performs the actual drawing step in the
    draw function below.

    \list
    \o calls draw(start, count) on this nodes geometry object (if any)
    \endlist

    Override this function to perform special processing on this node,
    after transformation & culling are applied and before sequencing of
    materials & effects are done; but just before (or after) the
    actual draw step.

    This default implementation simply draws the nodes geometry onto
    the painter.

    Example:
    \code
    void MySpecialNode::geometryDraw(QGLPainter *painter)
    {
        // at this point the node has survived culling, the model-view
        // matrix is transformed into this nodes frame of reference,
        // materials & effects have been applied as this node appears
        // in its place in the render sequence

        doMySpecialProcessing();

        // call parent implementation to do actual draw
        QGLSceneNode::geometryDraw(painter);
    }
    \endcode
*/
void QGLSceneNode::drawGeometry(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    if (d->count && d->geometry.count() > 0)
        d->geometry.draw(painter, d->start, d->count, d->drawingMode, d->drawingWidth);
}

/*!
    Draws this scene node and its children on the \a painter.

    In detail this function:
    \list
    \o ensures the effect specified by effect() is current on the painter
    \o sets the nodes materials onto the painter, if valid materials are present
    \o moves the model-view to the x, y, z position
    \o applies any local transforms() that may be set for this node
    \o calls draw() for all the child nodes
    \o calls draw(start, count) on this nodes geometry object (if any)
    \o restores the geometry's original materials if they were changed
    \o restores the model-view matrix if any local transform was applied
    \endlist

    Note that the draw() method does \bold not restore the effect.  If the first
    step above results in a change to the current QGL::Standard effect then it
    will remain set to that effect.  In general any painting method should
    ensure the effect it requires is current.

    The way draw is implemented ensures that this nodes effects, materials and
    transformations will apply by default to its child nodes.  Transformations
    are cumulative, but effects and materials override those of any parent node.

    Note that if the HideNode option is set for this node, neither it nor its
    children will be drawn.
*/
void QGLSceneNode::draw(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    if (d->options & HideNode)
        return;
    bool wasTransformed = false;

    QGLRenderSequencer *seq = painter->renderSequencer();

    if (seq->top() != this)
    {
        QMatrix4x4 m = transform();

        if (!m.isIdentity())
        {
            painter->modelViewMatrix().push();
            painter->modelViewMatrix() *= m;
            wasTransformed = true;
        }

        if (d->options & CullBoundingBox)
        {
            QBox3D bb = boundingBox();
            if (bb.isFinite() && !bb.isNull() && painter->isCullable(bb))
            {
                if (!d->culled && d->options & ReportCulling)
                {
                    d->culled = true;
                    emit culled();
                }
                if (wasTransformed)
                    painter->modelViewMatrix().pop();
                return;
            }
            else
            {
                if (d->culled && d->options & ReportCulling)
                {
                    d->culled = false;
                    emit displayed();
                }
            }
        }
    }

    if (seq->top() == NULL)
    {
        seq->setTop(this);
        while (true)
        {
            draw(painter);  // recursively draw myself for each state
            if (!seq->nextInSequence())
                break;
        }
        seq->reset();
    }
    else
    {
        bool stateEntered = false;
        if (d->childNodes.size() > 0)
        {
            seq->beginState(this);
            stateEntered = true;
            QList<QGLSceneNode*>::iterator cit = d->childNodes.begin();
            for ( ; cit != d->childNodes.end(); ++cit)
                (*cit)->draw(painter);
        }

        if (d->count && (d->geometry.count() > 0) && seq->renderInSequence(this))
        {
            bool idSaved = false;
            int id = -1;
            if (d->pickNode && painter->isPicking())
            {
                idSaved = true;
                id = painter->objectPickId();
                painter->setObjectPickId(d->pickNode->id());
            }

            if (!stateEntered)
            {
                stateEntered = true;
                seq->beginState(this);
            }
            seq->applyState();

            drawGeometry(painter);

            if (idSaved)
                painter->setObjectPickId(id);

            if (d->options & ViewNormals)
                drawNormalIndicators(painter);
        }
        if (stateEntered)
            seq->endState(this);
    }
    if (wasTransformed)
        painter->modelViewMatrix().pop();
}

/*!
    Returns the pick node for this scene node, if one was set; otherwise
    NULL (0) is returned.

    \sa setPickNode()
*/
QGLPickNode *QGLSceneNode::pickNode() const
{
    Q_D(const QGLSceneNode);
    return d->pickNode;
}

/*!
    Sets the pick node for this scene node to \a node.

    \sa pickNode()
*/
void QGLSceneNode::setPickNode(QGLPickNode *node)
{
    Q_D(QGLSceneNode);
    // TODO - resolve recursive picking - not supported by
    // color based pick AFAICT
    d->pickNode = node;
    if (node)
        node->setTarget(this);
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry, child nodes, and
    have all effects, transforms and other properties copied from this node.
    The only property that is not copied is pickNode().

    \sa cloneNoChildren()
*/
QGLSceneNode *QGLSceneNode::clone(QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = new QGLSceneNode
        (new QGLSceneNodePrivate(d), parent ? parent : this->parent());
    for (int index = 0; index < d->transforms.size(); ++index)
        node->addTransform(d->transforms.at(index)->clone(node));
    node->addNodes(d->childNodes);
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry, and
    have all effects, transforms and other properties copied from this node.
    The children() and pickNodes() are not cloned.

    \sa clone()
*/
QGLSceneNode *QGLSceneNode::cloneNoChildren(QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = new QGLSceneNode
        (new QGLSceneNodePrivate(d), parent ? parent : this->parent());
    for (int index = 0; index < d->transforms.size(); ++index)
        node->addTransform(d->transforms.at(index)->clone(node));
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have the same child nodes, except all child nodes
    whose objectName() is equal to \a name.

    \sa clone(), only()
*/
QGLSceneNode *QGLSceneNode::allExcept(const QString &name, QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = cloneNoChildren(parent);
    for (int index = 0; index < d->childNodes.count(); ++index) {
        QGLSceneNode *child = d->childNodes.at(index);
        if (child->objectName() != name)
            node->addNode(child);
    }
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have only one child node.  This child node will be
    the first child node of this one which has its objectName() equal to \a name.

    \sa clone(), allExcept()
*/
QGLSceneNode *QGLSceneNode::only(const QString &name, QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = cloneNoChildren(parent);
    for (int index = 0; index < d->childNodes.count(); ++index) {
        QGLSceneNode *child = d->childNodes.at(index);
        if (child->objectName() == name) {
            node->addNode(child);
            break;
        }
    }
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have the same child nodes, except all child nodes
    whose objectName() is in the list of \a names.

    \sa clone(), only()
*/
QGLSceneNode *QGLSceneNode::allExcept(const QStringList &names, QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = cloneNoChildren(parent);
    QSet<QString> chk = QSet<QString>::fromList(names);
    for (int index = 0; index < d->childNodes.count(); ++index) {
        QGLSceneNode *child = d->childNodes.at(index);
        if (!chk.contains(child->objectName()))
            node->addNode(child);
    }
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have only the child nodes from this
    whose objectName() is in the list of \a names.

    \sa clone(), allExcept()
*/
QGLSceneNode *QGLSceneNode::only(const QStringList &names, QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = cloneNoChildren(parent);
    QSet<QString> chk = QSet<QString>::fromList(names);
    for (int index = 0; index < d->childNodes.count(); ++index) {
        QGLSceneNode *child = d->childNodes.at(index);
        if (chk.contains(child->objectName()))
            node->addNode(child);
    }
    return node;
}

/*!
    \fn QGLSceneNode::updated()
    Signals that some property of this scene node, or one of its children,
    has changed in a manner that will require that the node be redrawn.
*/

/*!
    \fn QGLSceneNode::culled()
    Signals that the node was culled due to falling wholly outside the view
    frustum.  This signal can only fire if both QGLSceneNode::CullBoundingBox
    and QGLSceneNode::ReportCulling options are both set.
*/

/*!
    \fn QGLSceneNode::displayed()
    Signals that the node was displayed - or at least its geometry was sent
    to the GPU for rendering, since the GPU might still clip or occlude the
    node.  This signal can only fire if both QGLSceneNode::CullBoundingBox
    and QGLSceneNode::ReportCulling options are both set.
*/

/*!
    \relates QGLSceneNode
    Print a description of \a node, and all its descendants, to the console.
    This function uses qDebug for its output, so in the case of Windows the
    output will go to the debug window.  If QT_NO_DEBUG_OUTPUT or QT_NO_DEBUG
    has been defined, (as in release mode) this function will exit without
    doing anything.

    If \a detailed is true (which it is by default) then all the properties
    of each node are printed, including materials, geometry and transforms.

    If \a detailed is false, then just one line is printed with the name and
    some identifying information including a unique id for the node.

    The \a indent and \a loop parameters are used internally.
*/
void qDumpScene(QGLSceneNode *node, bool detailed, int indent, const QSet<QGLSceneNode *> &loop)
{
#if defined(QT_NO_DEBUG_STREAM) || defined(QT_NO_DEBUG) || defined(QT_NO_DEBUG_OUTPUT)
    Q_UNUSED(node);
    Q_UNUSED(detailed);
    Q_UNUSED(indent);
    Q_UNUSED(loop);
#else
#if !defined(QT_NO_THREAD)
    QCoreApplication *app = QApplication::instance();
    QThread *appThread = 0;
    if (app)
        appThread = QApplication::instance()->thread();
#endif
    QSet<QGLSceneNode *> lp = loop;
    lp.insert(node);
    QString ind;
    ind.fill(QLatin1Char(' '), indent * 4);
    if (detailed)
    {
        qDebug("\n%s ======== Node: %p - %s =========", qPrintable(ind), node,
               qPrintable(node->objectName()));
    }
    else
    {
        qDebug("\n%s Node: %p - %s", qPrintable(ind), node,
               qPrintable(node->objectName()));
        return;
    }
#if !defined(QT_NO_THREAD)
    if (appThread && appThread != node->thread())
        qDebug("\n%s        from thread: %p", qPrintable(ind), node->thread());
#endif
    qDebug("%s start: %d   count: %d   children:", qPrintable(ind), node->start(), node->count());
    {
        QList<QGLSceneNode*> children = node->children();
        QList<QGLSceneNode*>::const_iterator it = children.constBegin();
        for (int i = 0; it != children.constEnd(); ++it, ++i)
            qDebug("%s    %d: %p  ", qPrintable(ind), i, *it);
    }
    if (!node->position().isNull())
    {
        QVector3D p = node->position();
        qDebug("%s position: (%0.4f, %0.4f, %0.4f)", qPrintable(ind),
                p.x(), p.y(), p.z());
    }
    if (node->localTransform().isIdentity())
    {
        qDebug("%s local transform: identity", qPrintable(ind));
    }
    else
    {
        qDebug("%s local transform:", qPrintable(ind));
        QMatrix4x4 m = node->localTransform();
        for (int i = 0; i < 4; ++i)
            qDebug("%s     %0.4f   %0.4f   %0.4f   %0.4f",
                    qPrintable(ind), m(i, 0), m(i, 1), m(i, 2), m(i, 3));
    }
    QList<QGraphicsTransform3D*> tx = node->transforms();
    if (tx.size() > 0)
        qDebug("%s transforms list:", qPrintable(ind));
    for (int i = 0; i < tx.size(); ++i)
    {
        const QMetaObject *obj = tx.at(i)->metaObject();
        qDebug("%s     %s", qPrintable(ind), obj->className());
    }
    if (!node->geometry().isEmpty())
    {
        qDebug("%s geometry: %d indexes, %d vertices",
                qPrintable(ind), node->geometry().count(), node->geometry().count(QGL::Position));
    }
    else
    {
        qDebug("%s geometry: NULL", qPrintable(ind));
    }
    if (node->materialIndex() != -1)
    {
        qDebug("%s material: %d", qPrintable(ind), node->materialIndex());
        QGLMaterial *mat = node->material();
        QGLMaterialCollection *pal = node->palette();
        if (pal)
            qDebug("%s palette: %p", qPrintable(ind), pal);
        else
            qDebug("%s no palette", qPrintable(ind));
        if (pal)
        {
            mat = pal->material(node->materialIndex());
            if (mat)
                qDebug("%s mat name from pal: %s ", qPrintable(ind),
                        qPrintable(pal->material(node->materialIndex())->objectName()));
            else
                qDebug("%s indexed material %d does not exist in palette!",
                        qPrintable(ind), node->materialIndex());
        }
        if (mat)
        {
            QString mat_spx = QString(QLatin1String(" Amb: %1 - Diff: %2 - Spec: %3 - Shin: %4"))
                    .arg((mat->ambientColor().name()))
                    .arg(mat->diffuseColor().name())
                    .arg(mat->specularColor().name())
                    .arg(mat->shininess());
            if (mat->objectName().isEmpty())
                qDebug("%s    material pointer %p: %s", qPrintable(ind), mat, qPrintable(mat_spx));
            else
                qDebug("%s    \"%s\": %s", qPrintable(ind),
                        qPrintable(mat->objectName()),  qPrintable(mat_spx));
            for (int i = 0; i < mat->textureLayerCount(); ++i)
            {
                if (mat->texture(i) != 0)
                {
                    QGLTexture2D *tex = mat->texture(i);
                    if (tex->objectName().isEmpty())
                        qDebug("%s         texture %p", qPrintable(ind), tex);
                    else
                        qDebug("%s         texture %s", qPrintable(ind),
                                qPrintable(tex->objectName()));
                    QSize sz = tex->size();
                    qDebug(" - size: %d (w) x %d (h)", sz.width(), sz.height());
                }
            }
        }
        else
        {
            qDebug("%s - could not find indexed material!!", qPrintable(ind));
        }
    }
    else
    {
        qDebug("%s material: NONE", qPrintable(ind));
    }

    if (node->hasEffect())
    {
        if (node->userEffect())
        {
            qDebug("%s user effect %p", qPrintable(ind),
                    node->userEffect());
        }
        else
        {
            switch (node->effect())
            {
            case QGL::FlatColor:
                qDebug("%s flat color effect", qPrintable(ind)); break;
            case QGL::FlatPerVertexColor:
                qDebug("%s flat per vertex color effect", qPrintable(ind)); break;
            case QGL::FlatReplaceTexture2D:
                qDebug("%s flat replace texture 2D effect", qPrintable(ind)); break;
            case QGL::FlatDecalTexture2D:
                qDebug("%s flat decal texture 2D effect", qPrintable(ind)); break;
            case QGL::LitMaterial:
                qDebug("%s lit material effect", qPrintable(ind)); break;
            case QGL::LitDecalTexture2D:
                qDebug("%s lit decal texture 2D effect", qPrintable(ind)); break;
            case QGL::LitModulateTexture2D:
                qDebug("%s lit modulate texture 2D effect", qPrintable(ind)); break;
            }
        }
    }
    else
    {
        qDebug("%s no effect set", qPrintable(ind));
    }
    QList<QGLSceneNode*> children = node->children();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        if (!lp.contains(*it))
            qDumpScene(*it, indent + 1);
#endif
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLSceneNode &node)
{
    dbg << &node << "\n    start:" << node.start() << " count:" << node.count();
    QList<QGLSceneNode*> children = node.children();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        dbg << "\n        child:" << *it;

    if (node.localTransform().isIdentity())
        dbg << "\n    local transform: identity";
    else
        dbg << "\n    local transform:\n" << node.localTransform();

    if (node.geometry().count() > 0)
    {
        QGLMaterial *mat = node.material();
        QString mdesc;
        if (mat)
            mdesc = mat->objectName();
        dbg << "\n    geometry:" << node.geometry();
        dbg << "\n    material" << node.material() << ": " << mat << mdesc;
    }
    else
    {
        dbg << "\n    geometry: NULL";
        dbg << "\n    material" << node.material();
    }

    if (node.hasEffect())
    {
        if (node.userEffect())
        {
            dbg << "\n   user effect";
        }
        else
        {
            switch (node.effect())
            {
            case QGL::FlatColor:
                dbg << "\n    flat color effect"; break;
            case QGL::FlatPerVertexColor:
                dbg << "\n    flat per vertex color effect"; break;
            case QGL::FlatReplaceTexture2D:
                dbg << "\n    flat replace texture 2D effect"; break;
            case QGL::FlatDecalTexture2D:
                dbg << "\n    flat decal texture 2D effect"; break;
            case QGL::LitMaterial:
                dbg << "\n    lit material effect"; break;
            case QGL::LitDecalTexture2D:
                dbg << "\n    lit decal texture 2D effect"; break;
            case QGL::LitModulateTexture2D:
                dbg << "\n    lit modulate texture 2D effect"; break;
            }
        }
    }
    else
    {
        dbg << "\n    no effect set";
    }
    return dbg;
}

#endif // QT_NO_DEBUG_STREAM

QT_END_NAMESPACE
