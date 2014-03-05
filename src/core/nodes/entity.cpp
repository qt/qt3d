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

namespace Qt3D {

Entity::Entity(Node *parent)
    : Node(parent)
    , m_enabled(true)
{
}

QQmlListProperty<AbstractTransform> Entity::transformList()
{
    return QQmlListProperty<AbstractTransform>(this, 0,
                                  Entity::qmlAppendTransform,
                                  Entity::transformCount,
                                  Entity::transformAt,
                                  Entity::qmlClearTransforms);
}

QList<Component *> Entity::components() const
{
    // Construct the list of components by looking for all properties that
    // inherit from Component
    QList<Component *> componentList;
    const QMetaObject *meta = metaObject();
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty metaProperty = meta->property(i);
        const QVariant value = property(metaProperty.name());
        Component *component = value.value<Component *>();
        if (component)
            componentList.append(component);
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

QMatrix4x4 Entity::matrix() const
{
    if (m_transformsDirty) {
        m_matrix = applyTransforms();
        m_transformsDirty = false;
    }
    return m_matrix;
}

void Entity::update()
{
    if (m_transformsDirty) {
        m_matrix = applyTransforms();
        m_transformsDirty = false;
    }

    QMatrix4x4 prM;
    if (parentEntity())
        prM = parentEntity()->sceneMatrix();

    m_sceneMatrix = prM * matrix();
    // invalidate bounding volume information as required
}

void Entity::setMatrix(const QMatrix4x4 &m)
{
    qDeleteAll(m_transforms);
    m_transforms.clear();
    appendTransfrom(new MatrixTransform(m));
}

/*!
    The combined transform relative to the root of the scene
*/
QMatrix4x4 Entity::sceneMatrix() const
{
    return m_sceneMatrix;
}

/*!
    The center of rotation for the entity. Defaults to the local origin.
*/
QVector3D Entity::rotationCenter() const
{
    return QVector3D();
}

QList<AbstractTransform *> Entity::transforms() const
{
    return m_transforms;
}

void setRotationCenter(const QVector3D &rc)
{
    Q_UNUSED(rc);
}

void Entity::appendTransfrom(AbstractTransform *xform)
{
    m_transformsDirty = true;
    m_transforms.append( xform );
}

void Entity::removeTransform(AbstractTransform *xform)
{
    m_transformsDirty = true;
    m_transforms.removeOne( xform );
}

Entity *Entity::asEntity()
{
    return this;
}

Entity *Entity::parentEntity()
{
    return qobject_cast<Entity*>(parent());
}

QMatrix4x4 Entity::applyTransforms() const
{
    QMatrix4x4 m;
    m.setToIdentity();
    Q_FOREACH (AbstractTransform *t, m_transforms)
        m = t->matrix() * m;
    return m;
}


void Entity::qmlAppendTransform(QQmlListProperty<AbstractTransform> *list, AbstractTransform *obj  )
{
    if ( !obj )
        return;

    Entity* self = static_cast<Entity*>(list->object);
    self->appendTransfrom(obj);
}

AbstractTransform* Entity::transformAt(QQmlListProperty<AbstractTransform>* list, int index)
{
    Entity* self = static_cast<Entity*>(list->object);
    return self->transforms().at(index);
}

int Entity::transformCount(QQmlListProperty<AbstractTransform>* list)
{
    Entity* self = static_cast<Entity*>(list->object);
    return self->transforms().count();
}

void Entity::qmlClearTransforms(QQmlListProperty<AbstractTransform> *list)
{
    Entity* self = static_cast<Entity*>(list->object);
    qDeleteAll(self->m_transforms);
    self->m_transforms.clear();
    self->m_transformsDirty = true;
}


} // namespace Qt3D
