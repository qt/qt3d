// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRAYCASTINGSERVICE_P_H
#define QT3DRENDER_QRAYCASTINGSERVICE_P_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DRender/private/qabstractcollisionqueryservice_p.h>
#include <Qt3DRender/private/qray3d_p.h>

#include <QHash>
#include <QtConcurrent>
#include <QAtomicInt>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

class QBoundingVolumeProvider;
class QRayCastingServicePrivate;

typedef QFuture<QCollisionQueryResult> FutureQueryResult;

class Q_3DRENDERSHARED_EXPORT QRayCastingService : public QAbstractCollisionQueryService
{
public:
    QRayCastingService();

    QQueryHandle query(const QRay3D &ray, QueryMode mode, QBoundingVolumeProvider *provider) override;
    QCollisionQueryResult::Hit query(const QRay3D &ray, const QBoundingVolume *volume) override;

    QCollisionQueryResult fetchResult(const QQueryHandle &handle) override;
    QList<QCollisionQueryResult> fetchAllResults() const override;

protected:
    QRayCastingService(QRayCastingServicePrivate &dd);

private:
    Q_DISABLE_COPY(QRayCastingService)
    Q_DECLARE_PRIVATE(QRayCastingService)
};

class QRayCastingServicePrivate : public QAbstractCollisionQueryServicePrivate
{
public:
    QRayCastingServicePrivate(const QString &description);

    QCollisionQueryResult collides(const QRay3D &ray,
                                   QBoundingVolumeProvider *provider,
                                   QAbstractCollisionQueryService::QueryMode mode,
                                   const QQueryHandle &handle);
    QCollisionQueryResult::Hit collides(const QRay3D &ray,
                                        const QBoundingVolume *volume);

    Q_DECLARE_PUBLIC(QRayCastingService)

    struct Query
    {
        QQueryHandle handle;
        QRay3D ray;
        QRayCastingService::QueryMode mode;
    };

#if QT_CONFIG(concurrent)
    QHash<QQueryHandle, FutureQueryResult> m_results;
#else
    QHash<QQueryHandle, QCollisionQueryResult> m_results;
#endif
    QAtomicInt m_handlesCount;
};

} // namespace RayCasting
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRAYCASTINGSERVICE_P_H
