// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCLIP_P_H
#define QT3DANIMATION_QANIMATIONCLIP_P_H

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

#include <Qt3DAnimation/private/qabstractanimationclip_p.h>
#include <Qt3DAnimation/qanimationclip.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClipPrivate : public QAbstractAnimationClipPrivate
{
public:
    QAnimationClipPrivate();

    Q_DECLARE_PUBLIC(QAnimationClip)

    QAnimationClipData m_clipData;
};

struct QAnimationClipChangeData
{
    QAnimationClipData clipData;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCLIP_P_H
