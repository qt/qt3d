// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QCOMPONENT_P_H
#define QT3DCORE_QCOMPONENT_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORE_PRIVATE_EXPORT QComponentPrivate : public QNodePrivate
{
public:
    QComponentPrivate();
    ~QComponentPrivate();

    void addEntity(QEntity *entity);
    void removeEntity(QEntity *entity);

    Q_DECLARE_PUBLIC(QComponent)
    bool m_shareable;
    QList<QEntity *> m_entities;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QCOMPONENT_P_H
