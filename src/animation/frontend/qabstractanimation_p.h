// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTANIMATION_P_H
#define QT3DANIMATION_QABSTRACTANIMATION_P_H

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

#include <private/qobject_p.h>
#include <Qt3DAnimation/QAbstractAnimation>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class Q_AUTOTEST_EXPORT QAbstractAnimationPrivate : public QObjectPrivate
{
public:
    QAbstractAnimationPrivate(QAbstractAnimation::AnimationType type);

    QString m_animationName;
    QAbstractAnimation::AnimationType m_animationType;
    float m_position;
    float m_duration;

    Q_DECLARE_PUBLIC(QAbstractAnimation)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCONTROLLER_P_H
