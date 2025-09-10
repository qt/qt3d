// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qraycastingservice_p.h"

#include <Qt3DRender/private/qray3d_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/qboundingvolumeprovider_p.h>

#include <QtConcurrent>

#include "math.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace RayCasting {

namespace {

struct Hit
{
    Hit()
        : intersects(false)
        , distance(-1.0f)
    {}

    bool intersects;
    float distance;
    Qt3DCore::QNodeId id;
    Vector3D intersection;
    Vector3D uvw;
};

bool compareHitsDistance(const Hit &a, const Hit &b)
{
    return a.distance < b.distance;
}

Hit volumeRayIntersection(const QBoundingVolume *volume, const QRay3D &ray)
{
    Hit hit;
    if ((hit.intersects = volume->intersects(ray, &hit.intersection, &hit.uvw))) {
        hit.distance = ray.projectedDistance(hit.intersection);
        hit.id = volume->id();
    }
    return hit;
}

Hit reduceToFirstHit(Hit &result, const Hit &intermediate)
{
    if (intermediate.intersects) {
        if (result.distance == -1.0f ||
                (intermediate.distance >= 0.0f &&
                 intermediate.distance < result.distance))
            result = intermediate;
    }
    return result;
}

// Unordered
QList<Hit> reduceToAllHits(QList<Hit> &results, const Hit &intermediate)
{
    if (intermediate.intersects)
        results.push_back(intermediate);
    return results;
}

struct CollisionGathererFunctor
{
    QRay3D ray;

    typedef Hit result_type;

    Hit operator ()(const QBoundingVolume *volume) const
    {
        return volumeRayIntersection(volume, ray);
    }
};

} // anonymous


QCollisionQueryResult QRayCastingServicePrivate::collides(const QRay3D &ray, QBoundingVolumeProvider *provider,
                                                          QAbstractCollisionQueryService::QueryMode mode, const QQueryHandle &handle)
{
    Q_Q(QRayCastingService);

    const QList<QBoundingVolume *> volumes(provider->boundingVolumes());
    QCollisionQueryResult result;
    q->setResultHandle(result, handle);

    CollisionGathererFunctor gathererFunctor;
    gathererFunctor.ray = ray;

    if (mode == QAbstractCollisionQueryService::FirstHit) {
#if QT_CONFIG(concurrent)
        Hit firstHit = QtConcurrent::blockingMappedReduced<Hit>(volumes, gathererFunctor, reduceToFirstHit);
#else
        Hit firstHit;
        for (const QBoundingVolume *volume : volumes)
            firstHit = reduceToFirstHit(firstHit, gathererFunctor(volume));
#endif
        if (firstHit.intersects)
            q->addEntityHit(result, firstHit.id, firstHit.intersection, firstHit.distance, firstHit.uvw);
    } else {
#if QT_CONFIG(concurrent)
        QList<Hit> hits = QtConcurrent::blockingMappedReduced<QList<Hit>>(volumes, gathererFunctor, reduceToAllHits);
#else
        QList<Hit> hits;
        for (const QBoundingVolume *volume : volumes)
            hits = reduceToAllHits(hits, gathererFunctor(volume));
#endif
        std::sort(hits.begin(), hits.end(), compareHitsDistance);
        for (const Hit &hit : std::as_const(hits))
            q->addEntityHit(result, hit.id, hit.intersection, hit.distance, hit.uvw);
    }

    return result;
}

QCollisionQueryResult::Hit QRayCastingServicePrivate::collides(const QRay3D &ray, const QBoundingVolume *volume)
{
    QCollisionQueryResult::Hit result;
    Hit hit = volumeRayIntersection(volume, ray);
    if (hit.intersects)
    {
        result.m_distance = hit.distance;
        result.m_entityId = hit.id;
        result.m_intersection = hit.intersection;
        result.m_uvw = hit.uvw;
    }
    return result;
}

QRayCastingServicePrivate::QRayCastingServicePrivate(const QString &description)
    : QAbstractCollisionQueryServicePrivate(description)
    , m_handlesCount(0)
{
}

QRayCastingService::QRayCastingService()
    : QAbstractCollisionQueryService(*new QRayCastingServicePrivate(QStringLiteral("Collision detection service using Ray Casting")))
{
}

QQueryHandle QRayCastingService::query(const QRay3D &ray,
                                       QAbstractCollisionQueryService::QueryMode mode,
                                       QBoundingVolumeProvider *provider)
{
    Q_D(QRayCastingService);

    QQueryHandle handle = d->m_handlesCount.fetchAndStoreOrdered(1);


    // Blocking mapReduce

#if QT_CONFIG(concurrent)
    FutureQueryResult future = QtConcurrent::run([d, ray, provider, mode, handle]{
        return d->collides(ray, provider, mode, handle);
    });
    d->m_results.insert(handle, future);
#else
    d->m_results.insert(handle, d->collides(ray, provider, mode, handle));
#endif

    return handle;
}

QCollisionQueryResult::Hit QRayCastingService::query(const QRay3D &ray, const QBoundingVolume *volume)
{
    Q_D(QRayCastingService);

    return d->collides(ray, volume);
}

QCollisionQueryResult QRayCastingService::fetchResult(const QQueryHandle &handle)
{
    Q_D(QRayCastingService);

#if QT_CONFIG(concurrent)
    return d->m_results.value(handle).result();
#else
    return d->m_results.value(handle);
#endif
}

QList<QCollisionQueryResult> QRayCastingService::fetchAllResults() const
{
    Q_D(const QRayCastingService);

    QList<QCollisionQueryResult> results;
    results.reserve(d->m_results.size());

#if QT_CONFIG(concurrent)
    for (const FutureQueryResult &future : d->m_results)
        results.append(future.result());
#else
    for (const QCollisionQueryResult &result : d->m_results)
        results.append(result);
#endif

    return results;
}

} // namespace RayCasting
} // namespace Qt3DRender

QT_END_NAMESPACE
