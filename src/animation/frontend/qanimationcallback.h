/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

    virtual ~QAnimationCallback() { }

    virtual void valueChanged(const QVariant &value) = 0;
};

} // namespace Qt3DAnimation

Q_DECLARE_OPERATORS_FOR_FLAGS(Qt3DAnimation::QAnimationCallback::Flags)

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCALLBACK_H
