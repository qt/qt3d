// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCALLBACK_H
#define QT3DANIMATION_QANIMATIONCALLBACK_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class Q_3DANIMATIONSHARED_EXPORT QAnimationCallback
{
public:
    enum Flag {
        OnOwningThread = 0x0,
        OnThreadPool = 0x01
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    virtual ~QAnimationCallback();

    virtual void valueChanged(const QVariant &value) = 0;
};

} // namespace Qt3DAnimation

Q_DECLARE_OPERATORS_FOR_FLAGS(Qt3DAnimation::QAnimationCallback::Flags)

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCALLBACK_H
