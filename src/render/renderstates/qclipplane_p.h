// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCLIPPLANE_P_H
#define QT3DRENDER_QCLIPPLANE_P_H

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

#include <Qt3DRender/private/qrenderstate_p.h>
#include <Qt3DRender/qalphatest.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QClipPlanePrivate : public QRenderStatePrivate
{
public:
    QClipPlanePrivate()
        : QRenderStatePrivate(Render::ClipPlaneMask)
        , m_planeIndex(0)
        , m_normal()
        , m_distance(0.0f)
    {}

    int m_planeIndex;
    QVector3D m_normal;
    float m_distance;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCLIPPLANE_P_H
