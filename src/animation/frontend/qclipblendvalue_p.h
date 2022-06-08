// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QCLIPBLENDNODE_P_H
#define QT3DANIMATION_QCLIPBLENDNODE_P_H

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
#include <Qt3DAnimation/qclipblendvalue.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QClipBlendValuePrivate : public QAbstractClipBlendNodePrivate
{
public:
    QClipBlendValuePrivate();

    Q_DECLARE_PUBLIC(QClipBlendValue)

    Qt3DAnimation::QAbstractAnimationClip *m_clip;
};

struct QClipBlendValueData
{
    Qt3DCore::QNodeId clipId;
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCLIPBLENDNODE_P_H
