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

#include "quick3dentity.h"
#include <Qt3DCore/component.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

Quick3DEntity::Quick3DEntity(Node *parent)
    : Quick3DNode(parent)
    , Entity()
{
}

QQmlListProperty<Component> Quick3DEntity::componentList()
{
    return QQmlListProperty<Qt3D::Component>(this, 0,
                                             Quick3DEntity::qmlAppendComponent,
                                             Quick3DEntity::qmlComponentsCount,
                                             Quick3DEntity::qmlComponentAt,
                                             Quick3DEntity::qmlClearComponents);
}

Entity *Quick3DEntity::parentEntity()
{
    return qobject_cast<Entity*>(parent());
}

Entity *Quick3DEntity::asEntity()
{
    return this;
}

void Quick3DEntity::qmlAppendComponent(QQmlListProperty<Component> *list, Component *comp)
{
    if (comp == Q_NULLPTR)
        return;
    Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
    self->addComponent(comp);
}

Component *Quick3DEntity::qmlComponentAt(QQmlListProperty<Component> *list, int index)
{
    Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
    return self->components().at(index);
}

int Quick3DEntity::qmlComponentsCount(QQmlListProperty<Component> *list)
{
    Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
    return self->components().count();
}

void Quick3DEntity::qmlClearComponents(QQmlListProperty<Component> *list)
{
    Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
    ComponentList components = self->components();
    Q_FOREACH (Component *comp, components) {
        self->removeComponent(comp);
    }
}

} // Quick

} // Qt3D

QT_END_NAMESPACE
