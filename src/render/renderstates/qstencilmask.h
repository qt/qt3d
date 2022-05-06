/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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
