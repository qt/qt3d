// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QDISPATCHCOMPUTE_P_H
#define QT3DRENDER_QDISPATCHCOMPUTE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qframegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDispatchComputePrivate : public QFrameGraphNodePrivate
{
public:
    QDispatchComputePrivate()
        : QFrameGraphNodePrivate()
        , m_workGroupX(1)
        , m_workGroupY(1)
        , m_workGroupZ(1)
    {}

    int m_workGroupX;
    int m_workGroupY;
    int m_workGroupZ;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QDISPATCHCOMPUTE_P_H
