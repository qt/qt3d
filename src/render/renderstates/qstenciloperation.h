/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
