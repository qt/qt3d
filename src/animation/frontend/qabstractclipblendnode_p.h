// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DANIMATION_QABSTRACTCLIPBLENDNODE_P_H
#define QT3DANIMATION_QABSTRACTCLIPBLENDNODE_P_H

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

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractAnimationClip;
class QAbstractClipBlendNode;

class QAbstractClipBlendNodePrivate : public Qt3DCore::QNodePrivate
{
public:
    QAbstractClipBlendNodePrivate();

    Q_DECLARE_PUBLIC(QAbstractClipBlendNode)
    QList<QAbstractAnimationClip *> m_clips;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTCLIPBLENDNODE_P_H
