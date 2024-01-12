// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qentity.h"
#include "qentity_p.h"

#include <Qt3DCore/qcomponent.h>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <QQueue>

QT_BEGIN_NAMESPACE

namespace {

QString dumpNode(const Qt3DCore::QEntity *n) {
    auto formatNode = [](const Qt3DCore::QNode *n) {
        QString res = QString(QLatin1String("%1{%2}"))
                          .arg(QLatin1String(n->metaObject()->className()))
                          .arg(n->id().id());
        if (!n->objectName().isEmpty())
            res += QString(QLatin1String(" (%1)")).arg(n->objectName());
        if (!n->isEnabled())
            res += QLatin1String(" [D]");
        return res;
    };

    QString res = formatNode(n);
    const auto &components = n->components();
    if (components.size()) {
        QStringList componentNames;
        for (const auto &c : components)
            componentNames += formatNode(c);
        res += QString(QLatin1String(" [ %1 ]")).arg(componentNames.join(QLatin1String(", ")));
    }

    return res;
}

QStringList dumpSG(const Qt3DCore::QNode *n, int level = 0)
{
    QStringList reply;
    const auto *entity = qobject_cast<const Qt3DCore::QEntity *>(n);
    if (entity != nullptr) {
        QString res = dumpNode(entity);
        reply += res.rightJustified(res.size() + level * 2, QLatin1Char(' '));
        level++;
    }

    const auto children = n->childNodes();
    for (auto *child: children)
        reply += dumpSG(child, level);

    return reply;
}

}

namespace Qt3DCore {

/*!
    \class Qt3DCore::QEntity
    \inmodule Qt3DCore
    \inherits Qt3DCore::QNode
    \since 5.5

    \brief Qt3DCore::QEntity is a Qt3DCore::QNode subclass that can aggregate several
    Qt3DCore::QComponent instances that will specify its behavior.

    By itself a Qt3DCore::QEntity is an empty shell. The behavior of a Qt3DCore::QEntity
    object is defined by the Qt3DCore::QComponent objects it references. Each Qt3D
    backend aspect will be able to interpret and process an Entity by
    recognizing which components it is made up of. One aspect may decide to only
    process entities composed of a single Qt3DCore::QTransform component whilst
    another may focus on Qt3DInput::QMouseHandler.

    \sa Qt3DCore::QComponent, Qt3DCore::QTransform
 */

/*!
    \fn template<typename T> QList<T *> Qt3DCore::QEntity::componentsOfType() const

    Returns all the components added to this entity that can be cast to
    type T or an empty vector if there are no such components.
*/

/*! \internal */
QEntityPrivate::QEntityPrivate()
    : QNodePrivate()
    , m_parentEntityId()
    , m_dirty(false)
{}

/*! \internal */
QEntityPrivate::~QEntityPrivate()
{
}

/*! \internal */
QEntityPrivate *QEntityPrivate::get(QEntity *q)
{
    return q->d_func();
}

/*! \internal */
void QEntityPrivate::removeDestroyedComponent(QComponent *comp)
{
    // comp is actually no longer a QComponent, just a QObject

    Q_CHECK_PTR(comp);
    qCDebug(Nodes) << Q_FUNC_INFO << comp;

    updateComponentRelationShip(comp, ComponentRelationshipChange::Removed);
    m_components.removeOne(comp);
    m_dirty = true;

    // Remove bookkeeping connection
    unregisterDestructionHelper(comp);
}

/*!
    Constructs a new Qt3DCore::QEntity instance with \a parent as parent.
 */
QEntity::QEntity(QNode *parent)
    : QEntity(*new QEntityPrivate, parent) {}

/*! \internal */
QEntity::QEntity(QEntityPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    connect(this, &QNode::parentChanged, this, &QEntity::onParentChanged);
}

QEntity::~QEntity()
{
    // remove all component aggregations
    Q_D(QEntity);
    // to avoid hammering m_components by repeated removeComponent()
    // calls below, move all contents out, so the removeOne() calls in
    // removeComponent() don't actually remove something:
    const auto components = std::move(d->m_components);
    for (QComponent *comp : components)
        removeComponent(comp);
}


/*!
    \typedef Qt3DCore::QComponentVector
    \relates Qt3DCore::QEntity

    List of QComponent pointers.
 */

/*!
    Returns the list of Qt3DCore::QComponent instances the entity is referencing.
 */
QComponentVector QEntity::components() const
{
    Q_D(const QEntity);
    return d->m_components;
}

/*!
    Adds a new reference to the component \a comp.

    \note If the Qt3DCore::QComponent has no parent, the Qt3DCore::QEntity will set
    itself as its parent thereby taking ownership of the component.
 */
void QEntity::addComponent(QComponent *comp)
{
    Q_D(QEntity);
    Q_CHECK_PTR( comp );
    qCDebug(Nodes) << Q_FUNC_INFO << comp;

    // A Component can only be aggregated once
    if (d->m_components.count(comp) != 0)
        return ;

    // We need to add it as a child of the current node if it has been declared inline
    // Or not previously added as a child of the current node so that
    // 1) The backend gets notified about it's creation
    // 2) When the current node is destroyed, it gets destroyed as well
    if (!comp->parent())
        comp->setParent(this);

    QNodePrivate::get(comp)->_q_ensureBackendNodeCreated();

    d->m_components.append(comp);
    d->m_dirty = true;

    // Ensures proper bookkeeping
    d->registerPrivateDestructionHelper(comp, &QEntityPrivate::removeDestroyedComponent);

    d->updateComponentRelationShip(comp, ComponentRelationshipChange::Added);
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

    d->updateComponentRelationShip(comp, ComponentRelationshipChange::Removed);

    d->m_components.removeOne(comp);
    d->m_dirty = true;

    // Remove bookkeeping connection
    d->unregisterDestructionHelper(comp);
}

/*!
    Returns the parent Qt3DCore::QEntity instance of this entity. If the
    immediate parent isn't a Qt3DCore::QEntity, this function traverses up the
    scene hierarchy until a parent Qt3DCore::QEntity is found. If no
    Qt3DCore::QEntity parent can be found, returns null.
 */
QEntity *QEntity::parentEntity() const
{
    Q_D(const QEntity);
    QNode *parentNode = QNode::parentNode();
    QEntity *parentEntity = qobject_cast<QEntity *>(parentNode);

    while (parentEntity == nullptr && parentNode != nullptr) {
        parentNode = parentNode->parentNode();
        parentEntity = qobject_cast<QEntity*>(parentNode);
    }
    if (!parentEntity) {
        if (!d->m_parentEntityId.isNull())
            d->m_parentEntityId = QNodeId();
    } else {
        if (d->m_parentEntityId != parentEntity->id())
            d->m_parentEntityId = parentEntity->id();
    }
    return parentEntity;
}

/*
    \internal

    Returns the Qt3DCore::QNodeId id of the parent Qt3DCore::QEntity instance of the
    current Qt3DCore::QEntity object. The QNodeId isNull method will return true if
    there is no Qt3DCore::QEntity parent of the current Qt3DCore::QEntity in the scene
    hierarchy.
 */
QNodeId QEntityPrivate::parentEntityId() const
{
    Q_Q(const QEntity);
    if (m_parentEntityId.isNull())
        q->parentEntity();
    return m_parentEntityId;
}

QString QEntityPrivate::dumpSceneGraph() const
{
    Q_Q(const QEntity);
    return dumpSG(q).join(QLatin1Char('\n'));
}

void QEntityPrivate::updateComponentRelationShip(QComponent *component, ComponentRelationshipChange::RelationShip change)
{
    if (m_changeArbiter) {
        // Ensure node has its postConstructorInit called if we reach this
        // point, we could otherwise endup referencing a node that has yet
        // to be created in the backend
        QNodePrivate::get(component)->_q_ensureBackendNodeCreated();

        Q_Q(QEntity);
        m_changeArbiter->addDirtyEntityComponentNodes(q, component, change);
    }
}

void QEntity::onParentChanged(QObject *)
{
    Q_D(QEntity);
    if (!d->m_hasBackendNode)
        return;

    d->update();
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qentity.cpp"
