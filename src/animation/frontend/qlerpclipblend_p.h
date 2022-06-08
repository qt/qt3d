// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QLERPBLEND_P_H
#define QT3DANIMATION_QLERPBLEND_P_H

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

#include <Qt3DAnimation/private/qabstractclipblendnode_p.h>
#include <Qt3DCore/qnodeid.h>
#include "qlerpclipblend.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QLerpClipBlend;

class QLerpClipBlendPrivate : public QAbstractClipBlendNodePrivate
{
public:
    QLerpClipBlendPrivate();

    Q_DECLARE_PUBLIC(QLerpClipBlend)
    QAbstractClipBlendNode *m_startClip;
    QAbstractClipBlendNode *m_endClip;
    float m_blendFactor;
};

struct QLerpClipBlendData
{
    Qt3DCore::QNodeId startClipId;
    Qt3DCore::QNodeId endClipId;
    float blendFactor;
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QLERPBLEND_P_H
