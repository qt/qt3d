// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILOPERATION_H
#define QT3DRENDER_QSTENCILOPERATION_H

#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qstenciloperationarguments.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilOperationPrivate;

class Q_3DRENDERSHARED_EXPORT QStencilOperation : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QStencilOperationArguments *front READ front CONSTANT)
    Q_PROPERTY(Qt3DRender::QStencilOperationArguments *back READ back CONSTANT)

public:
    explicit QStencilOperation(Qt3DCore::QNode *parent = nullptr);
    ~QStencilOperation();

    QStencilOperationArguments *front() const;
    QStencilOperationArguments *back() const;

private:
    Q_DECLARE_PRIVATE(QStencilOperation)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILOPERATION_H
