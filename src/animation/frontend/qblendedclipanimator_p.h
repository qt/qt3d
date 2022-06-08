// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H
#define QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H

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

#include <Qt3DAnimation/private/qabstractclipanimator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractClipBlendNode;

class QBlendedClipAnimatorPrivate : public Qt3DAnimation::QAbstractClipAnimatorPrivate
{
public:
    QBlendedClipAnimatorPrivate();

    Q_DECLARE_PUBLIC(QBlendedClipAnimator)
    QAbstractClipBlendNode *m_blendTreeRoot;
};

struct QBlendedClipAnimatorData : public QAbstractClipAnimatorData
{
    Qt3DCore::QNodeId blendTreeRootId;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H
