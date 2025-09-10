// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSCISSORTEST_P_H
#define QT3DRENDER_QSCISSORTEST_P_H

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
#include <Qt3DRender/qscissortest.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QScissorTestPrivate : public QRenderStatePrivate
{
public:
    QScissorTestPrivate()
        : QRenderStatePrivate(Render::ScissorStateMask)
        , m_left(0)
        , m_bottom(0)
        , m_width(0)
        , m_height(0)
    {
    }

    Q_DECLARE_PUBLIC(QScissorTest)
    int m_left;
    int m_bottom;
    int m_width;
    int m_height;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCISSORTEST_P_H
