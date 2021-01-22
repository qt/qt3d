/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
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
****************************************************************************/

#ifndef QT3DRENDER_QSTENCILTEST_H
#define QT3DRENDER_QSTENCILTEST_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilTestPrivate;
class QStencilTestArguments;

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
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QSTENCILTEST_H
