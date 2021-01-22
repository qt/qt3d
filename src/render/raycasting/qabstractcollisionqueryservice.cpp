/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
