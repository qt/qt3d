// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK3DGEOMETRY_P_H
#define QT3D_QUICK3DGEOMETRY_P_H

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

#include <Qt3DCore/QGeometry>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DGeometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QAttribute> attributes READ attributeList)
    Q_CLASSINFO("DefaultProperty", "attributes")

public:
    explicit Quick3DGeometry(QObject *parent = nullptr);
    inline QGeometry *parentGeometry() const { return qobject_cast<QGeometry *>(parent()); }

    QQmlListProperty<Qt3DCore::QAttribute> attributeList();

private:
    QList<Qt3DCore::QAttribute *> m_managedAttributes;
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK3DGEOMETRY_P_H
