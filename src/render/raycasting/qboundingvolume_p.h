// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QBOUNDINGVOLUME_P_H
#define QT3DRENDER_QBOUNDINGVOLUME_P_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/vector3d_p.h>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
namespace RayCasting {

class QRay3D;

class Q_3DRENDERSHARED_EXPORT QBoundingVolume
{
public:
    QBoundingVolume();
    virtual ~QBoundingVolume();

    enum Type {
        Sphere = 0,
        Triangle
    };

    virtual Qt3DCore::QNodeId id() const = 0;
    virtual bool intersects(const QRay3D &ray, Vector3D *q = nullptr,
                            Vector3D *uvw = nullptr) const = 0;

    virtual Type type() const = 0;
};

} // namespace RayCasting
} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::RayCasting::QBoundingVolume*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QBOUNDINGVOLUME_P_H
