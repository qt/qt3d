// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERSTATESET_H
#define QT3DRENDER_QRENDERSTATESET_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderStateSetPrivate;
class QRenderState;

class Q_3DRENDERSHARED_EXPORT QRenderStateSet : public QFrameGraphNode
{
    Q_OBJECT

public:
    explicit QRenderStateSet(Qt3DCore::QNode *parent = nullptr);
    ~QRenderStateSet();

    void addRenderState(QRenderState *state);
    void removeRenderState(QRenderState *state);
    QList<QRenderState *> renderStates() const;

protected:
    explicit QRenderStateSet(QRenderStateSetPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QRenderStateSet)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERSTATESET_H
