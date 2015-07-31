/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcomponent.h"
#include "qcomponent_p.h"
#include "qentity.h"
#include "qentity_p.h"
#include "qscene_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QComponentPrivate
    \internal
*/
QComponentPrivate::QComponentPrivate()
    : QNodePrivate()
    , m_shareable(true)
    , m_enabled(true)
{
}

void QComponentPrivate::addEntity(QEntity *entity)
{
    m_entities.append(entity);

    if (m_scene != Q_NULLPTR && !m_scene->hasEntityForComponent(m_id, entity->id())) {
        if (!m_shareable && !m_scene->entitiesForComponent(m_id).isEmpty())
            qWarning() << "Trying to assign a non shareable component to more than one Entity";
        m_scene->addEntityForComponent(m_id, entity->id());
    }

    // We notify only if we have a QChangeArbiter
    if (m_changeArbiter != Q_NULLPTR) {
        Q_Q(QComponent);
        QScenePropertyChangePtr e(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, q->id()));
        e->setPropertyName("entity");
        e->setValue(QVariant::fromValue(entity->id()));
        notifyObservers(e);
    }
}

void QComponentPrivate::removeEntity(QEntity *entity)
{
    // We notify only if we have a QChangeArbiter
    if (m_changeArbiter != Q_NULLPTR) {
        Q_Q(QComponent);
        QScenePropertyChangePtr e(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, q->id()));
        e->setPropertyName("entity");
        e->setValue(QVariant::fromValue(entity->id()));
        notifyObservers(e);
    }

    if (m_scene != Q_NULLPTR)
        m_scene->removeEntityForComponent(m_id, entity->id());

    m_entities.removeAll(entity);
}

/*!
    \class Qt3D::QComponent
    \inmodule Qt3DCore
    \inherits Qt3D::QNode
    \since 5.5

    \brief The base class of scene nodes that can be aggregated by Qt3D::QEntity
    instances as a component.

    A Qt3D::QComponent provides a vertical slice of behavior that can be assigned to and
    sometimes shared across Qt3D::QEntity instances.

    Qt3D::QComponent subclasses are often aggregated in groups that impart useful
    behavior to the aggregating entity. For example, to have an Entity that gets
    drawn by the Qt3D renderer aspect, an entity would most likely aggregate
    Qt3D::QTransform, Qt3D::QMesh, and Qt3D::QMaterial components.

    \sa Qt3D::QEntity
*/

/*!
    Constructs a new QComponent instance with \a parent as the parent.
    \note a QComponent should never be instanced directly,
    instance one of the subclasses instead.
*/
QComponent::QComponent(QNode *parent)
    : QNode(*new QComponentPrivate, parent)
{
}

QComponent::~QComponent()
{
    Q_ASSERT_X(QNodePrivate::get(this)->m_wasCleanedUp, Q_FUNC_INFO, "QNode::cleanup should have been called by now. A Qt3D::QComponent subclass didn't call QNode::cleanup in its destructor");

    Q_FOREACH (QEntity *entity, entities()) {
        QEntityPrivate *entityPimpl = static_cast<QEntityPrivate *>(QEntityPrivate::get(entity));
        if (entityPimpl)
            entityPimpl->m_components.removeAll(this);
    }
}

/*!
    \return whether the QComponent is shareable across entities or not.
*/
bool QComponent::shareable() const
{
    Q_D(const QComponent);
    return d->m_shareable;
}

/*!
    Returns whether the QComponent is enabled or not.
*/
bool QComponent::isEnabled() const
{
    Q_D(const QComponent);
    return d->m_enabled;
}

/*!
    Set the QComponent to enabled if \a enabled is true.
    By default a Qt3D::QComponent is always enabled.

    \note the interpretation of what enabled means is aspect-dependent. Even if
    enabled is set to false, some aspects may still consider the component in
    some manner. This is documented on a class by class basis.
*/
void QComponent::setEnabled(bool enabled)
{
    Q_D(QComponent);
    if (d->m_enabled != enabled) {
        d->m_enabled = enabled;
        emit enabledChanged();
    }
}

/*!
    The QComponent can be shared across several entities if \a shareable is true.
*/
void QComponent::setShareable(bool shareable)
{
    Q_D(QComponent);
    if (d->m_shareable != shareable) {
        d->m_shareable = shareable;
        emit shareableChanged();
    }
}

void QComponent::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QComponent *comp = static_cast<const QComponent *>(ref);
    setEnabled(comp->isEnabled());
    setShareable(comp->shareable());
}

/*!
    \return a QVector containing all the entities that reference this component.
*/
QVector<QEntity *> QComponent::entities() const
{
    Q_D(const QComponent);
    return d->m_entities;
}

/*! \internal */
QComponent::QComponent(QComponentPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // namespace Qt3D

/*!
    \qmltype Component3D
    \instantiates Qt3D::QComponent
    \inqmlmodule Qt3D
    \inherits Node
    \since 5.5
*/

/*!
    \qmlproperty bool Qt3D::Component3D::shareable
*/

/*!
    \qmlproperty bool Qt3D::Component3D::enabled
*/

QT_END_NAMESPACE
