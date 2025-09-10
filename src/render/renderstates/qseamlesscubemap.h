// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSEAMLESSCUBEMAP_H
#define QT3DRENDER_QSEAMLESSCUBEMAP_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSeamlessCubemapPrivate;

class Q_3DRENDERSHARED_EXPORT QSeamlessCubemap : public QRenderState
{
    Q_OBJECT

public:
    explicit QSeamlessCubemap(Qt3DCore::QNode *parent = nullptr);
    ~QSeamlessCubemap();

private:
    Q_DECLARE_PRIVATE(QSeamlessCubemap)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSEAMLESSCUBEMAP_H
