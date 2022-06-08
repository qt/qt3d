// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QENTITY_P_H
#define QT3DCORE_QENTITY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qentity.h>

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORE_PRIVATE_EXPORT QEntityPrivate : public QNodePrivate
{
public :
    QEntityPrivate();
    ~QEntityPrivate();

    Q_DECLARE_PUBLIC(QEntity)
    static QEntityPrivate *get(QEntity *q);

    QNodeId parentEntityId() const;
    void updateComponentRelationShip(QComponent *component, ComponentRelationshipChange::RelationShip change);

    template<class T>
    QList<T*> componentsOfType() const
    {
        QList<T*> typedComponents;
        for (QComponent *comp : m_components) {
            T *typedComponent = qobject_cast<T*>(comp);
            if (typedComponent != nullptr)
                typedComponents.append(typedComponent);
        }
        return typedComponents;
    }

    QString dumpSceneGraph() const;
    void removeDestroyedComponent(QComponent *comp);

    QComponentVector m_components;
    mutable QNodeId m_parentEntityId;
    bool m_dirty;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QENTITY_P_H
