// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILTEST_P_H
#define QT3DRENDER_QSTENCILTEST_P_H

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
#include <Qt3DRender/qstenciltest.h>
#include <Qt3DRender/qstenciltestarguments.h>
#include <Qt3DRender/private/qstenciltestarguments_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct QStencilTestData
{
    QStencilTestArgumentsData front;
    QStencilTestArgumentsData back;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT QStencilTestPrivate : public QRenderStatePrivate
{
public:
    QStencilTestPrivate()
        : QRenderStatePrivate(Render::StencilTestStateMask)
        , m_front(new QStencilTestArguments(QStencilTestArguments::Front))
        , m_back(new QStencilTestArguments(QStencilTestArguments::Back))
    {
    }

    Q_DECLARE_PUBLIC(QStencilTest)
    QStencilTestArguments *m_front;
    QStencilTestArguments *m_back;

    void fillData(QStencilTestData &data) const;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QStencilTestData)

#endif // QT3DRENDER_QSTENCILTEST_P_H
