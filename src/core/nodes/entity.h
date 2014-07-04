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

#ifndef QT3D_ENTITY_H
#define QT3D_ENTITY_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>

#include <QMetaType>
#include <QUuid>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Component;
class Entity;
class EntityPrivate;

typedef QList<Component*> ComponentList;

class QT3DCORESHARED_EXPORT Entity : public QNode
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit Entity(QNode *parent = 0);

    const QUuid uuid() const;

    ComponentList components() const;

    template <class T>
    QList<T*> componentsOfType() const
    {
        QList<T*> result;
        foreach (Component* comp, components()) {
            T* i = qobject_cast<T*>(comp);
            if (i)
                result.append(i);
        }

        return result;
    }

    template <class T>
    static T* findComponentInTree(QNode* root)
    {
        if (!root)
            return Q_NULLPTR;

        if (root->asEntity()) {
            foreach (Component* comp, root->asEntity()->components()) {
                T* i = qobject_cast<T*>(comp);
                if (i)
                    return i;
            } // of component iteration
        } // of is-entity

        foreach (QNode* child, root->children()) {
            T* i = findComponentInTree<T>(child);
            if (i)
                return i;
        } // of child nodes iteration

        return Q_NULLPTR;
    }

    template <class T>
    static T* findEntityInTree(QNode* root)
    {
        if (!root)
            return Q_NULLPTR;

        if (root->asEntity()) {
            foreach (QNode* child, root->children()) {
                if (!qobject_cast<Entity*>(child))
                    continue;
                T* i = qobject_cast<T*>(child);
                if (i)
                    return i;
            } // of child iteration
        } // of is-entity

        foreach (QNode* child, root->children()) {
            T* i = findEntityInTree<T>(child);
            if (i)
                return i;
        } // of child nodes iteration

        return Q_NULLPTR;
    }

    virtual void addComponent(Component *comp);
    virtual void removeComponent(Component *comp);

    bool isEnabled() const;
    void setEnabled(bool on);

    virtual Entity *parentEntity();
    virtual Entity *asEntity() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void enabledChanged();

protected:
    Q_DECLARE_PRIVATE(Entity)
    Entity(EntityPrivate &dd, QNode *parent = 0);
};

} // namespace Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Entity *)

#endif // QT3D_ENTITY_H
