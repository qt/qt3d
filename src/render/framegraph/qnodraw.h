// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DRENDER_QNODRAW_H
#define QT3DRENDER_QNODRAW_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QNoDraw : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QNoDraw(Qt3DCore::QNode *parent = nullptr);
    ~QNoDraw();
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QNODRAW_H
