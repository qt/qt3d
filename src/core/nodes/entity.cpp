/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "entity.h"

#include "component.h"
#include "abstracttransform.h"
#include "matrixtransform.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

Entity::Entity(Node *parent)
    : Node(parent),
      QObservable()
    , m_enabled(true)
{
    m_uuid = QUuid::createUuid();
}

QList<Component *> Entity::components() const
{
    // Construct the list of components by looking for all properties that
    // inherit from Component
    QList<Component *> componentList;
    const QMetaObject *meta = metaObject();
    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
        qDebug() << QString::fromLatin1(meta->method(i).methodSignature());
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty metaProperty = meta->property(i);
        const QVariant value = property(metaProperty.name());
        Component *component = value.value<Component *>();
        if (component) {
            // Connect notify signal of the property to the updatePropertySlots
            // This allows aspects to monitor for property changes
            if (metaProperty.hasNotifySignal()) {
                QMetaMethod componentPropertyUpdateSlot = metaObject()->method(metaObject()->indexOfSlot("componentPropertyUpdated()"));
                qDebug() << metaProperty.name() << metaProperty.notifySignal().methodSignature();
                QObject::connect(this, metaProperty.notifySignal(),
                                 this, componentPropertyUpdateSlot,
                                 Qt::UniqueConnection);
            }
            componentList.append(component);
        }
    }
    return componentList + m_components;
}

void Entity::addComponent(Component *comp)
{
    Q_CHECK_PTR( comp );
    Q_ASSERT(m_components.count(comp) == 0);
    comp->setParent(this);
    m_components.append(comp);
}

void Entity::removeComponent(Component *comp)
{
    Q_CHECK_PTR(comp);
    m_components.removeOne(comp);
}

bool Entity::isEnabled() const
{
    return m_enabled;
}

void Entity::setEnabled(bool on)
{
    if (m_enabled != on) {
        m_enabled = on;
        emit enabledChanged();
    }
}

void Entity::update()
{
    //    if (m_transformsDirty) {
    //        m_matrix = applyTransforms();
    //        m_transformsDirty = false;
    //    }

    //    QMatrix4x4 prM;
    //    if (parentEntity())
    //        prM = parentEntity()->sceneMatrix();

    //    m_sceneMatrix = prM * matrix();
    // invalidate bounding volume information as required
}

void Entity::componentPropertyUpdated()
{
    qDebug() << Q_FUNC_INFO << "Component property updated ";

    QScenePropertyChangePtr propertyChange(new QScenePropertyChange(AllChanges, this));
    notifyObservers(propertyChange);
}

Entity *Entity::asEntity()
{
    return this;
}

Entity *Entity::parentEntity()
{
    return qobject_cast<Entity*>(parent());
}

} // namespace Qt3D

QT_END_NAMESPACE
