// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QNOPICKING_H
#define QT3DRENDER_QNOPICKING_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QNoPicking : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QNoPicking(Qt3DCore::QNode *parent = nullptr);
    ~QNoPicking();
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QNOPICKING_H
