/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qcomponentremovedchange.h"
#include "qcomponentremovedchange_p.h"

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QComponentRemovedChangePrivate::QComponentRemovedChangePrivate(const QEntity *entity,
                                                               const QComponent *component)
    : QSceneChangePrivate()
    , m_entityId(entity->id())
    , m_componentId(component->id())
    , m_componentMetaObject(QNodePrivate::findStaticMetaObject(component->metaObject()))
{
}

/*!
 * \class Qt3DCore::QComponentRemovedChange
 * \inheaderfile Qt3DCore/QComponentRemovedChange
 * \inherits Qt3DCore::QSceneChange
 * \inmodule Qt3DCore
 * \brief The QComponentRemovedChange class is used to notify when a component is removed from an entity.
 *
 */

/*!
 * \typedef Qt3DCore::QComponentRemovedChangePtr
 * \relates Qt3DCore::QComponentRemovedChange
 *
 * A shared pointer for QComponentRemovedChange.
 */

/*!
 * Constructs a new QComponentRemovedChange which will notify \a entity that \a component was removed.
 */
QComponentRemovedChange::QComponentRemovedChange(const QEntity *entity,
                                                 const QComponent *component)
    : QSceneChange(*new QComponentRemovedChangePrivate(entity, component),
                   ComponentRemoved, entity->id())
{
}

/*!
 * Constructs a new QComponentRemovedChange which will notify \a component that it was removed from \a entity
 */
QComponentRemovedChange::QComponentRemovedChange(const QComponent *component,
                                                 const QEntity *entity)
    : QSceneChange(*new QComponentRemovedChangePrivate(entity, component),
                   ComponentRemoved, component->id())
{

}

QComponentRemovedChange::~QComponentRemovedChange()
{
}

/*!
  \return the id of the entity the component was removed from.
 */
QNodeId QComponentRemovedChange::entityId() const Q_DECL_NOTHROW
{
    Q_D(const QComponentRemovedChange);
    return d->m_entityId;
}

/*!
  \return the id of the component removed.
 */
QNodeId QComponentRemovedChange::componentId() const Q_DECL_NOTHROW
{
    Q_D(const QComponentRemovedChange);
    return d->m_componentId;
}

/*!
 * \return the metaobject.
 */
const QMetaObject *QComponentRemovedChange::componentMetaObject() const Q_DECL_NOTHROW
{
    Q_D(const QComponentRemovedChange);
    return d->m_componentMetaObject;
}

QT_END_NAMESPACE

} // namespace Qt3DCore

