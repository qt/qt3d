/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qraycastingservice.h"
#include "qraycastingservice_p.h"

#include <Qt3DCore/qray3d.h>
#include <Qt3DRenderer/sphere.h>
#include <Qt3DCore/qboundingvolumeprovider.h>

#include <QMap>
#include <QtConcurrent>

#include "math.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QCollisionQueryResult QRayCastingServicePrivate::collides(const QRay3D &ray, QBoundingVolumeProvider *provider,
                                                          QAbstractCollisionQueryService::QueryMode mode, const QQueryHandle &handle)
{
    Q_Q(QRayCastingService);

    const QVector<QBoundingVolume *> spheres(provider->boundingVolumes());
    QCollisionQueryResult result;
    q->setResultHandle(result, handle);

    if (mode == QAbstractCollisionQueryService::FirstHit) {
        QVector3D intersection;
        float collidingPointDistance = -1.0f;
        QNodeId collidingSphere;

        Q_FOREACH (QBoundingVolume *bs, spheres) {
            if (bs->intersects(ray, &intersection)) {
                const float distanceFromRay = ray.projectedDistance(intersection);

                if (collidingPointDistance == -1.0f || (distanceFromRay >= 0.0f && distanceFromRay < collidingPointDistance)) {
                    collidingPointDistance = distanceFromRay;
                    collidingSphere = bs->id();
                }
            }
        }

        if (!collidingSphere.isNull()) {
            q->addEntityHit(result, collidingSphere);
        }

    } else if (mode == QAbstractCollisionQueryService::AllHits) {
        Q_FOREACH (QBoundingVolume *bs, spheres) {
            if (bs->intersects(ray))
                q->addEntityHit(result, bs->id());
        }
    }
    return result;
}

QRayCastingServicePrivate::QRayCastingServicePrivate(const QString &description, QBoundingVolumeProvider *provider)
    : QAbstractCollisionQueryServicePrivate(description)
    , m_handlesCount(0)
    , m_boundingProvider(provider)
{
}

QRayCastingService::QRayCastingService(QBoundingVolumeProvider *provider)
    : QAbstractCollisionQueryService(*new QRayCastingServicePrivate(QStringLiteral("Collision detection service using Ray Casting"),
                                                                    provider))
{
}

QQueryHandle QRayCastingService::query(const QRay3D &ray, QAbstractCollisionQueryService::QueryMode mode)
{
    Q_D(QRayCastingService);

    QQueryHandle handle = d->m_handlesCount.fetchAndStoreOrdered(1);

    // TODO: Implement run using jobs to avoid multiple threadpool
    FutureQueryResult future = QtConcurrent::run(d, &QRayCastingServicePrivate::collides,
                                                 ray, d->m_boundingProvider, mode, handle);
    d->m_results.insert(handle, future);

    return handle;
}

QCollisionQueryResult QRayCastingService::fetchResult(const QQueryHandle &handle)
{
    Q_D(QRayCastingService);

    return d->m_results.value(handle).result();
}

QVector<QCollisionQueryResult> QRayCastingService::fetchAllResults() const
{
    Q_D(const QRayCastingService);

    const QList<FutureQueryResult> futureResults = d->m_results.values();
    QVector<QCollisionQueryResult> results;
    results.reserve(futureResults.size());

    Q_FOREACH (const FutureQueryResult &future, futureResults)
        results.append(future.result());

    return results;
}

} // Qt3D

QT_END_NAMESPACE
