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

#include "qentity.h"
#include "qentity_p.h"
#include "qcomponent.h"
#include "qcomponent_p.h"
#include "qabstracttransform.h"
#include "qmatrixtransform.h"

#include <Qt3DCore/qsceneinterface.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/corelogging_p.h>
#include <QMetaObject>
#include <QMetaProperty>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QEntityPrivate
    \internal
*/
QEntityPrivate::QEntityPrivate()
    : QNodePrivate()
    , m_enabled(true)
{}

/*!
    \class Qt3D::QEntity
    \inmodule Qt3DCore
    \inherits Qt3D::QNode
    \since 5.5

    \brief Qt3D::QEntity is a Qt3D::QNode subclass that can aggregate several
    Qt3D::QComponent instances that will specify its behavior.

    By itself a Qt3D::QEntity is an empty shell. The behavior of a Qt3D::QEntity
    object is defined by the Qt3D::QComponent objects it references. Each Qt3D
    backend aspect will be able to interpret and process an Entity by
    recognizing which components it is made up of. One aspect may decide to only
    process entities composed of a single Qt3D::QTransform component whilst
    another may focus on Qt3D::QMouseInput.

    \sa Qt3D::QComponent, Qt3D::QTransform
*/

/*!
    Constructs a new Qt3D::QEntity instance with \a parent as parent.
*/
QEntity::QEntity(QNode *parent)
    : QNode(*new QEntityPrivate, parent)
{
}

QEntity::~QEntity()
{
    // If all children are removed
    // That includes the components that are parented by this entity

    // We need to call removeAllComponent only for real Entities (not clone)
    if (d_func()->m_changeArbiter)
        removeAllComponents();
}

/*! \internal */
QEntity::QEntity(QEntityPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    Copies all the properties and components of the Qt3D::QEntity \a ref to the
    current instance.
*/
void QEntity::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QEntity *entity = static_cast<const QEntity*>(ref);
    d_func()->m_enabled = entity->d_func()->m_enabled;
    d_func()->m_visible = entity->d_func()->m_visible;

    Q_FOREACH (QComponent *c, entity->d_func()->m_components) {
        QNode *ccclone = QNode::clone(c);
        addComponent(qobject_cast<QComponent *>(ccclone));
    }
}

/*!
    Returns the list of Qt3D::QComponent instances the entity is referencing.
*/
QList<QComponent *> QEntity::components() const
{
    Q_D(const QEntity);
    return d->m_components;
}

/*!
    Adds a new reference to the component \a comp.

    \note If the Qt3D::QComponent has no parent, the Qt3D::QEntity will set
    itself as its parent thereby taking ownership of the component.
*/
void QEntity::addComponent(QComponent *comp)
{
    Q_D(QEntity);
    Q_CHECK_PTR( comp );
    qCDebug(Nodes) << Q_FUNC_INFO << comp;
    Q_ASSERT(d->m_components.count(comp) == 0);
    d->m_components.append(comp);
    // We only set the Entity as the Component's parent when it has no parent
    // This will be the case mostly on C++ but rarely in QML
    if (!comp->parent())
        comp->setParent(this);

    if (d->m_scene != Q_NULLPTR) {
        if (!comp->shareable() && !d->m_scene->entitiesForComponent(comp->id()).isEmpty())
            qWarning() << "Trying to assign a non shareable component to more than one Entity";
        d->m_scene->addEntityForComponent(comp->id(), d->m_id);
    }

    if (d->m_changeArbiter != Q_NULLPTR) {
        // Sending a full fledged component in the notification as we'll need
        // to know which type of component it was and its properties to create
        // the backend object
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentAdded, QSceneChange::Node, id()));
        propertyChange->setPropertyName("component");
        propertyChange->setValue(QVariant::fromValue(QNodePtr(QNode::clone(comp), &QNodePrivate::nodePtrDeleter)));
        d->notifyObservers(propertyChange);
    }
    static_cast<QComponentPrivate *>(QComponentPrivate::get(comp))->addEntity(this);
}

/*!
    Removes the reference to \a comp.
*/
void QEntity::removeComponent(QComponent *comp)
{
    Q_CHECK_PTR(comp);
    qCDebug(Nodes) << Q_FUNC_INFO << comp;
    Q_D(QEntity);

    static_cast<QComponentPrivate *>(QComponentPrivate::get(comp))->removeEntity(this);

    if (d->m_changeArbiter != Q_NULLPTR) {
        // Sending just the component id as it is the only part needed to
        // cleanup the backend object. This way we avoid a clone which might
        // fail in the case of large scenes.
        QScenePropertyChangePtr propertyChange(new QScenePropertyChange(ComponentRemoved, QSceneChange::Node, id()));
        propertyChange->setValue(QVariant::fromValue(comp->id()));
        propertyChange->setPropertyName("componentId");
        d->notifyObservers(propertyChange);
    }

    if (d->m_scene != Q_NULLPTR)
        d->m_scene->removeEntityForComponent(comp->id(), d->m_id);

    d->m_components.removeOne(comp);
}

/*!
    Remove all references to the components.
*/
void QEntity::removeAllComponents()
{
    Q_D(const QEntity);
    Q_FOREACH (QComponent *comp, d->m_components)
        removeComponent(comp);
}

/*!
    Returns the parent Qt3D::QEntity instance of this entity. If the
    immediate parent isn't a Qt3D::QEntity, this function traverses up the
    scene hierarchy until a parent Qt3D::QEntity is found. If no
    Qt3D::QEntity parent can be found, returns null.
*/
QEntity *QEntity::parentEntity() const
{
    QNode *parentNode = QNode::parentNode();
    QEntity *parentEntity = qobject_cast<QEntity *>(parentNode);

    while (parentEntity == Q_NULLPTR && parentNode != Q_NULLPTR) {
        parentNode = parentNode->parentNode();
        parentEntity = qobject_cast<QEntity*>(parentNode);
    }
    return parentEntity;
}

} // namespace Qt3D

QT_END_NAMESPACE
