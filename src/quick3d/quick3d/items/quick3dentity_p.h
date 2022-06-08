// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_QUICK3DENTITY_P_H
#define QT3D_QUICK_QUICK3DENTITY_P_H

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
#include <QtQml/QQmlListProperty>

#include <Qt3DQuick/private/quick3dnode_p.h>
#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QEntity;
class QComponent;

namespace Quick {

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DEntity : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QComponent> components READ componentList)

public:
    explicit Quick3DEntity(QObject *parent = nullptr);
    QQmlListProperty<Qt3DCore::QComponent> componentList();

    inline QEntity *parentEntity() const { return qobject_cast<QEntity*>(parent()); }

private:
    QList<Qt3DCore::QComponent *> m_managedComponents;
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QUICK3DENTITY_P_H
