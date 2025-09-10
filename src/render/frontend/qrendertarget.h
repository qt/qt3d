// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERTARGET_H
#define QT3DRENDER_QRENDERTARGET_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTargetPrivate;
class QRenderTargetOutput;

class Q_3DRENDERSHARED_EXPORT QRenderTarget : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit QRenderTarget(Qt3DCore::QNode *parent = nullptr);
    ~QRenderTarget();

    void addOutput(QRenderTargetOutput *output);
    void removeOutput(QRenderTargetOutput *output);
    QList<QRenderTargetOutput *> outputs() const;

protected:
    explicit QRenderTarget(QRenderTargetPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderTarget)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERTARGET_H
