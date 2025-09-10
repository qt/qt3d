// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILOPERATION_P_H
#define QT3DRENDER_QSTENCILOPERATION_P_H

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
#include <Qt3DRender/qstenciloperation.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/private/qstenciloperationarguments_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct QStencilOperationData
{
    QStencilOperationArgumentsData front;
    QStencilOperationArgumentsData back;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT QStencilOperationPrivate : public QRenderStatePrivate
{
public:
    QStencilOperationPrivate()
        : QRenderStatePrivate(Render::StencilOpMask)
        , m_front(new QStencilOperationArguments(QStencilOperationArguments::Front, q_ptr))
        , m_back(new QStencilOperationArguments(QStencilOperationArguments::Back, q_ptr))
    {}
    ~QStencilOperationPrivate();

    QStencilOperationArguments *m_front;
    QStencilOperationArguments *m_back;

    void fillData(QStencilOperationData &data) const;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QStencilOperationData)

#endif // QT3DRENDER_QSTENCILOPERATION_P_H
