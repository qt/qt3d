// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QALPHATEST_P_H
#define QT3DRENDER_QALPHATEST_P_H

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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAlphaTestPrivate : public QRenderStatePrivate
{
public:
    QAlphaTestPrivate()
        : QRenderStatePrivate(Render::AlphaTestMask)
        , m_alphaFunction(QAlphaTest::Never)
        , m_referenceValue(0.0f)
    {
    }

    Q_DECLARE_PUBLIC(QAlphaTest)
    QAlphaTest::AlphaFunction m_alphaFunction;
    float m_referenceValue;
};

struct QAlphaTestData
{
    QAlphaTest::AlphaFunction alphaFunction;
    float referenceValue;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QALPHATEST_P_H
