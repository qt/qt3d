// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DRENDER_QFRUSTUMCULLING_H
#define QT3DRENDER_QFRUSTUMCULLING_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QFrustumCulling : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QFrustumCulling(Qt3DCore::QNode *parent = nullptr);
    ~QFrustumCulling();
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QFRUSTUMCULLING_H
