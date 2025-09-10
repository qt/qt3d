// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILMASK_H
#define QT3DRENDER_QSTENCILMASK_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilMaskPrivate;

class Q_3DRENDERSHARED_EXPORT QStencilMask : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(uint frontOutputMask READ frontOutputMask WRITE setFrontOutputMask NOTIFY frontOutputMaskChanged)
    Q_PROPERTY(uint backOutputMask READ backOutputMask WRITE setBackOutputMask NOTIFY backOutputMaskChanged)
public:
    explicit QStencilMask(Qt3DCore::QNode *parent = nullptr);
    ~QStencilMask();

    uint frontOutputMask() const;
    uint backOutputMask() const;

public Q_SLOTS:
    void setFrontOutputMask(uint frontOutputMask);
    void setBackOutputMask(uint backOutputMask);

Q_SIGNALS:
    void frontOutputMaskChanged(uint frontOutputMask);
    void backOutputMaskChanged(uint backOutputMask);

private:
    Q_DECLARE_PRIVATE(QStencilMask)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILMASK_H
