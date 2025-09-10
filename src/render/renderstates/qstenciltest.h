// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILTEST_H
#define QT3DRENDER_QSTENCILTEST_H

#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qstenciltestarguments.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilTestPrivate;

class Q_3DRENDERSHARED_EXPORT QStencilTest : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QStencilTestArguments *front READ front CONSTANT)
    Q_PROPERTY(Qt3DRender::QStencilTestArguments *back READ back CONSTANT)
public:
    explicit QStencilTest(Qt3DCore::QNode *parent = nullptr);
    ~QStencilTest();

    QStencilTestArguments *front() const;
    QStencilTestArguments *back() const;

private:
    Q_DECLARE_PRIVATE(QStencilTest)
};

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QSTENCILTEST_H
