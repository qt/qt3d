// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_QUICK3DNODE_P_H
#define QT3D_QUICK_QUICK3DNODE_P_H

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

#include <Qt3DCore/qnode.h>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QNode> childNodes READ childNodes)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit Quick3DNode(QObject *parent = 0);

    QQmlListProperty<QObject> data();
    QQmlListProperty<Qt3DCore::QNode> childNodes();

    inline QNode *parentNode() const { return qobject_cast<QNode*>(parent()); }

private Q_SLOTS:
    void childAppended(int idx, QObject *child);
    void childRemoved(int idx, QObject *child);
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QUICK3DNODE_P_H
