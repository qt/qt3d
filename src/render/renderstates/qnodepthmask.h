// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QNODEPTHMASK_H
#define QT3DRENDER_QNODEPTHMASK_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QNoDepthMaskPrivate;

class Q_3DRENDERSHARED_EXPORT  QNoDepthMask : public QRenderState
{
    Q_OBJECT
public:
    explicit QNoDepthMask(Qt3DCore::QNode *parent = nullptr);
    ~QNoDepthMask();

private:
    Q_DECLARE_PRIVATE(QNoDepthMask)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QNODEPTHMASK_H
