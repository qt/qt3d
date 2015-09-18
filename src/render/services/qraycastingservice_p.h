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

#ifndef QT3DRENDER_QRAYCASTINGSERVICE_P_H
#define QT3DRENDER_QRAYCASTINGSERVICE_P_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qabstractcollisionqueryservice.h>
#include <Qt3DCore/private/qabstractcollisionqueryservice_p.h>
#include <Qt3DCore/qray3d.h>

#include <QHash>
#include <QtConcurrent>
#include <QAtomicInt>

QT_BEGIN_NAMESPACE

namespace Qt3D {
class QBoundingVolume;
class QBoundingVolumeProvider;
}

namespace Qt3DRender {

typedef QFuture<Qt3D::QCollisionQueryResult> FutureQueryResult;

class QRayCastingServicePrivate : public Qt3D::QAbstractCollisionQueryServicePrivate
{
public:
    QRayCastingServicePrivate(const QString &description, Qt3D::QBoundingVolumeProvider *provider);

    Qt3D::QCollisionQueryResult collides(const Qt3D::QRay3D &ray,
                                         Qt3D::QBoundingVolumeProvider *provider,
                                         Qt3D::QAbstractCollisionQueryService::QueryMode mode,
                                         const Qt3D::QQueryHandle &handle);

    Q_DECLARE_PUBLIC(QRayCastingService)

    struct Query
    {
        Qt3D::QQueryHandle handle;
        Qt3D::QRay3D ray;
        QRayCastingService::QueryMode mode;
    };

    QHash<Qt3D::QQueryHandle, FutureQueryResult> m_results;
    QAtomicInt m_handlesCount;

    Qt3D::QBoundingVolumeProvider *m_boundingProvider;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRAYCASTINGSERVICE_P_H
