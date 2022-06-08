// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_QBOUNDINGVOLUMEPROVIDER_P_H
#define QT3DRENDER_QBOUNDINGVOLUMEPROVIDER_P_H

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
#include <QList>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {

class QBoundingVolume;

class Q_3DRENDERSHARED_EXPORT QBoundingVolumeProvider
{
public:
    virtual ~QBoundingVolumeProvider();
    virtual QList<QBoundingVolume *> boundingVolumes() const = 0;
};

} // namespace RayCasting
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBOUNDINGVOLUMEPROVIDER_P_H
