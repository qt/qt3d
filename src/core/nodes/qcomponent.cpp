// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcomponent.h"
#include "qcomponent_p.h"

#include <Qt3DCore/qentity.h>

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QComponentPrivate::QComponentPrivate()
    : QNodePrivate()
    , m_shareable(true)
{
}

QComponentPrivate::~QComponentPrivate()
{
}

void QComponentPrivate::addEntity(QEntity *entity)
{
    Q_Q(QComponent);
    m_entities.append(entity);

    if (m_scene != nullptr && !m_scene->hasEntityForComponent(m_id, entity->id())) {
        if (!m_shareable && !m_scene->entitiesForComponent(m_id).isEmpty())
            qWarning() << "Trying to assign a non shareable component to more than one Entity";
        m_scene->addEntityForComponent(m_id, entity->id());
    }

    Q_EMIT q->addedToEntity(entity);
}

void QComponentPrivate::removeEntity(QEntity *entity)
{
    Q_Q(QComponent);
    if (m_scene != nullptr)
        m_scene->removeEntityForComponent(m_id, entity->id());

    m_entities.removeAll(entity);

    Q_EMIT q->removedFromEntity(entity);
}

/*!
    \class Qt3DCore::QComponent
    \inmodule Qt3DCore
    \inherits Qt3DCore::QNode
    \since 5.5

    \brief The base class of scene nodes that can be aggregated by Qt3DCore::QEntity
    instances as a component.

    A Qt3DCore::QComponent provides a vertical slice of behavior that can be assigned to and
    sometimes shared across Qt3DCore::QEntity instances.

    Qt3DCore::QComponent subclasses are often aggregated in groups that impart useful
    behavior to the aggregating entity. For example, to have an Entity that gets
    drawn by the Qt3D renderer aspect, an entity would most likely aggregate
    Qt3DCore::QTransform, Qt3DRender::QMesh, and Qt3DRender::QMaterial components.

    \sa Qt3DCore::QEntity
*/

/*!
    \fn Qt3DCore::QComponent::addedToEntity(Qt3DCore::QEntity *entity)

    Indicates that a reference has been added to \a entity.
*/
/*!
    \fn Qt3DCore::QComponent::removedFromEntity(Qt3DCore::QEntity *entity)

    Indicates that a reference has been removed from \a entity.

*/
/*!
    Constructs a new QComponent instance with \a parent as the parent.
    \note a QComponent should never be instanced directly,
    instance one of the subclasses instead.
*/
QComponent::QComponent(QNode *parent)
    : QComponent(*new QComponentPrivate, parent) {}

QComponent::~QComponent()
{
    Q_D(QComponent);

    // iterate on copy since removeEntity removes from the list, invalidating the iterator
    const auto entities = std::move(d->m_entities);
    for (QEntity *entity : entities) {
        QEntityPrivate *entityPimpl = static_cast<QEntityPrivate *>(QEntityPrivate::get(entity));
        if (entityPimpl)
            entityPimpl->m_components.removeAll(this);
        d->removeEntity(entity);
    }
}

/*!
    \property Qt3DCore::QComponent::isShareable
    Holds the shareable flag of the QComponent. The QComponent can be shared across several
    entities if \c{true}.
*/
bool QComponent::isShareable() const
{
    Q_D(const QComponent);
    return d->m_shareable;
}

void QComponent::setShareable(bool shareable)
{
    Q_D(QComponent);
    if (d->m_shareable != shareable) {
        d->m_shareable = shareable;
        emit shareableChanged(shareable);
    }
}

/*!
    Returns a QList containing all the entities that reference this component.
*/
QList<QEntity *> QComponent::entities() const
{
    Q_D(const QComponent);
    return d->m_entities;
}

/*! \internal */
QComponent::QComponent(QComponentPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // namespace Qt3DCore

/*!
    \qmltype Component3D
    \nativetype Qt3DCore::QComponent
    \inqmlmodule Qt3D.Core
    \inherits Node
    \since 5.5
    \brief Provides the base type for creating Qt 3D components.

    \TODO
*/

/*!
    \qmlproperty bool Component3D::isShareable
*/

QT_END_NAMESPACE

#include "moc_qcomponent.cpp"
