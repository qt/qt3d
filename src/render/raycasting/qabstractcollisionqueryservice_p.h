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

#include <QVector>

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/qabstractserviceprovider_p.h>
#include <Qt3DRender/private/qcollisionqueryresult_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QRay3D;
}

namespace Qt3DRender {

class QBoundingVolumeProvider;

class QAbstractCollisionQueryServicePrivate : public Qt3DCore::QAbstractServiceProviderPrivate
{
public:
    QAbstractCollisionQueryServicePrivate(const QString &description)
        : QAbstractServiceProviderPrivate(Qt3DCore::QServiceLocator::CollisionService, description)
    {}
};

class QT3DRENDERSHARED_EXPORT QAbstractCollisionQueryService : public Qt3DCore::QAbstractServiceProvider
{
public:
    enum QueryMode {
        FirstHit,
        AllHits
    };

    virtual QQueryHandle query(const Qt3DCore::QRay3D &ray, QueryMode mode, QBoundingVolumeProvider *provider) = 0;

    virtual QCollisionQueryResult fetchResult(const QQueryHandle &handle) = 0;
    virtual QVector<QCollisionQueryResult> fetchAllResults() const = 0;

protected:
    QAbstractCollisionQueryService(const QString &description = QString());
    QAbstractCollisionQueryService(QAbstractCollisionQueryServicePrivate &dd);

    void setResultHandle(QCollisionQueryResult &result, const QQueryHandle &handle);
    void addEntityHit(QCollisionQueryResult &result, const Qt3DCore::QNodeId &entity);

private:
    Q_DECLARE_PRIVATE(QAbstractCollisionQueryService)
};

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QAbstractCollisionQueryService::QueryMode)

#endif // QT3DRENDER_QABSTRACTCOLLISIONQUERYSERVICE_P_H
