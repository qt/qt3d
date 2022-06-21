// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractcollisionqueryservice_p.h"

#include "qcollisionqueryresult_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

QAbstractCollisionQueryService::QAbstractCollisionQueryService(const QString &description)
    : QAbstractServiceProvider(*new QAbstractCollisionQueryServicePrivate(description))
{
}

QAbstractCollisionQueryService::QAbstractCollisionQueryService(QAbstractCollisionQueryServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{
}

void QAbstractCollisionQueryService::setResultHandle(QCollisionQueryResult &result, const QQueryHandle &handle)
{
    result.d_func()->setHandle(handle);
}

void QAbstractCollisionQueryService::addEntityHit(QCollisionQueryResult &result, Qt3DCore::QNodeId entity,
                                                  const Vector3D& intersection, float distance,
                                                  const Vector3D& uvw)
{
    result.d_func()->addEntityHit(entity, intersection, distance, uvw);
}

} // RayCasting
} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qabstractcollisionqueryservice_p.cpp"
