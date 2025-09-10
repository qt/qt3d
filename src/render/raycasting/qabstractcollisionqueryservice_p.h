// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTCOLLISIONQUERYSERVICE_P_H
#define QT3DRENDER_QABSTRACTCOLLISIONQUERYSERVICE_P_H

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

#include <QList>

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/qabstractserviceprovider_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

class QRay3D;
class QBoundingVolume;
class QBoundingVolumeProvider;

class QAbstractCollisionQueryServicePrivate : public Qt3DCore::QAbstractServiceProviderPrivate
{
public:
    QAbstractCollisionQueryServicePrivate(const QString &description)
        : QAbstractServiceProviderPrivate(Qt3DCore::QServiceLocator::CollisionService, description)
    {}
};

class Q_3DRENDERSHARED_EXPORT QAbstractCollisionQueryService : public Qt3DCore::QAbstractServiceProvider
{
    Q_OBJECT
public:
    enum QueryMode {
        FirstHit,
        AllHits
    };

    virtual QQueryHandle query(const QRay3D &ray, QueryMode mode, QBoundingVolumeProvider *provider) = 0;
    virtual QCollisionQueryResult::Hit query(const QRay3D &ray, const QBoundingVolume* volume) = 0;

    virtual QCollisionQueryResult fetchResult(const QQueryHandle &handle) = 0;
    virtual QList<QCollisionQueryResult> fetchAllResults() const = 0;

protected:
    QAbstractCollisionQueryService(const QString &description = QString());
    QAbstractCollisionQueryService(QAbstractCollisionQueryServicePrivate &dd);

    void setResultHandle(QCollisionQueryResult &result, const QQueryHandle &handle);
    void addEntityHit(QCollisionQueryResult &result, Qt3DCore::QNodeId entity, const Vector3D &intersection,
                      float distance, const Vector3D &uvw);

private:
    Q_DECLARE_PRIVATE(QAbstractCollisionQueryService)
};

} // RayCasting
} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::RayCasting::QAbstractCollisionQueryService::QueryMode) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QABSTRACTCOLLISIONQUERYSERVICE_P_H
