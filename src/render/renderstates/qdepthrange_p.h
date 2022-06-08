// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QDEPTHRANGE_P_H
#define QT3DRENDER_QDEPTHRANGE_P_H

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

class Q_3DRENDERSHARED_PRIVATE_EXPORT QDepthRangePrivate : public QRenderStatePrivate
{
public:
    QDepthRangePrivate()
        : QRenderStatePrivate(Render::DepthRangeMask)
        , m_nearValue(0.0f)
        , m_farValue(1.0f)
    {}

    double m_nearValue;
    double m_farValue;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QDEPTHRANGE_P_H
