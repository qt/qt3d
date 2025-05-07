// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QDEBUGOVERLOAY_H
#define QDEBUGOVERLOAY_H

#include <Qt3DRender/QFrameGraphNode>
#include <QtGui/QImage>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDebugOverlayPrivate;

class Q_3DRENDERSHARED_EXPORT QDebugOverlay : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QDebugOverlay(Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QDebugOverlay)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QDEBUGOVERLOAY_H
