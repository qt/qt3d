// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeometry.h"
#include "qgeometryfactory_p.h"
#include "qgeometry_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qattribute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QGeometryFactory::~QGeometryFactory()
{
}

QGeometryPrivate::QGeometryPrivate()
    : QNodePrivate(),
      m_boundingVolumePositionAttribute(nullptr)
{
}

QGeometryPrivate::~QGeometryPrivate()
{
}

QGeometryPrivate *QGeometryPrivate::get(QGeometry *q)
{
    return q->d_func();
}

void QGeometryPrivate::setScene(QScene *scene)
{
    QNodePrivate::setScene(scene);
    if (scene)
        scene->markDirty(QScene::GeometryDirty);
}

void QGeometryPrivate::update()
{
    if (!m_blockNotifications) {
        m_dirty = true;
        markDirty(QScene::GeometryDirty);
    }
    QNodePrivate::update();
}

void QGeometryPrivate::setExtent(const QVector3D &minExtent, const QVector3D &maxExtent)
{
    Q_Q(QGeometry);
    if (m_minExtent != minExtent) {
        m_minExtent = minExtent;
        const auto wasBlocked = q->blockNotifications(true);
        emit q->minExtentChanged(minExtent);
        q->blockNotifications(wasBlocked);
    }

    if (m_maxExtent != maxExtent) {
        m_maxExtent = maxExtent;
        const auto wasBlocked = q->blockNotifications(true);
        emit q->maxExtentChanged(maxExtent);
        q->blockNotifications(wasBlocked);
    }
}

/*!
    \qmltype Geometry
    \instantiates Qt3DCore::QGeometry
    \inqmlmodule Qt3D.Core
    \inherits Node
    \since 5.7
    \brief Encapsulates geometry.

    A Geometry type is used to group a list of Attribute objects together
    to form a geometric shape Qt3D is able to render using GeometryRenderer.
    Special attribute can be set in order to calculate bounding volume of the shape.
 */

/*!
    \class Qt3DCore::QGeometry
    \inmodule Qt3DCore
    \since 5.7
    \brief Encapsulates geometry.

    A Qt3DCore::QGeometry class is used to group a list of Qt3DCore::QAttribute
    objects together to form a geometric shape Qt3D is able to render using
    Qt3DCore::QGeometryRenderer. Special attribute can be set in order to calculate
    bounding volume of the shape.
 */

/*!
    \qmlproperty Attribute Geometry::boundingVolumePositionAttribute

    Holds the attribute used to compute the bounding volume. The bounding volume is used internally
    for picking and view frustum culling.

    If unspecified, the system will look for the attribute using the name returned by
    QAttribute::defaultPositionAttributeName.

    \sa Attribute
 */
/*!
    \qmlproperty list<Attribute> Geometry::attributes

    Holds the list of attributes the geometry comprises of.
 */

/*!
    \property QGeometry::boundingVolumePositionAttribute

    Holds the attribute used to compute the bounding volume. The bounding volume is used internally
    for picking and view frustum culling.

    If unspecified, the system will look for the attribute using the name returned by
    QAttribute::defaultPositionAttributeName.

    \sa Qt3DCore::QAttribute
 */


/*!
    Constructs a new QGeometry with \a parent.
 */
QGeometry::QGeometry(QNode *parent)
    : QGeometry(*new QGeometryPrivate(), parent) {}

/*!
    \internal
 */
QGeometry::~QGeometry()
{
}

/*!
    \internal
 */
QGeometry::QGeometry(QGeometryPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    \fn void Qt3DCore::QGeometry::addAttribute(Qt3DCore::QAttribute *attribute)
    Adds an \a attribute to this geometry.
 */
void QGeometry::addAttribute(QAttribute *attribute)
{
    Q_ASSERT(attribute);
    Q_D(QGeometry);
    if (!d->m_attributes.contains(attribute)) {
        d->m_attributes.append(attribute);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(attribute, &QGeometry::removeAttribute, d->m_attributes);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!attribute->parent())
            attribute->setParent(this);

        d->update();
    }
}

/*!
    \fn void Qt3DCore::QGeometry::removeAttribute(Qt3DCore::QAttribute *attribute)
    Removes the given \a attribute from this geometry.
 */
void QGeometry::removeAttribute(QAttribute *attribute)
{
    Q_ASSERT(attribute);
    Q_D(QGeometry);
    if (!d->m_attributes.removeOne(attribute))
        return;
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(attribute);
    d->update();
}

void QGeometry::setBoundingVolumePositionAttribute(QAttribute *boundingVolumePositionAttribute)
{
    Q_D(QGeometry);
    if (d->m_boundingVolumePositionAttribute != boundingVolumePositionAttribute) {
        d->m_boundingVolumePositionAttribute = boundingVolumePositionAttribute;
        emit boundingVolumePositionAttributeChanged(boundingVolumePositionAttribute);
    }
}

QAttribute *QGeometry::boundingVolumePositionAttribute() const
{
    Q_D(const QGeometry);
    return d->m_boundingVolumePositionAttribute;
}

/*!
    \qmlproperty vector3d Geometry::minExtent

    Holds the vertex with the lowest x, y, z position values.
 */

/*!
    \property QGeometry::minExtent

    Holds the vertex with the lowest x, y, z position values.
 */
QVector3D QGeometry::minExtent() const
{
    Q_D(const QGeometry);
    return d->m_minExtent;
}

/*!
    \qmlproperty vector3d Geometry::maxExtent

    Holds the vertex with the highest x, y, z position values.
 */

/*!
    \property QGeometry::maxExtent

    Holds the vertex with the highest x, y, z position values.
 */
QVector3D QGeometry::maxExtent() const
{
    Q_D(const QGeometry);
    return d->m_maxExtent;
}

/*!
    Returns the list of attributes in this geometry.
 */
QList<QAttribute *> QGeometry::attributes() const
{
    Q_D(const QGeometry);
    return d->m_attributes;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qgeometry.cpp"
