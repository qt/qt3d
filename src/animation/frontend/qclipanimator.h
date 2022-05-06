/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DANIMATION_QCLIPANIMATOR_H
#define QT3DANIMATION_QCLIPANIMATOR_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qabstractclipanimator.h>
#include <Qt3DAnimation/qabstractanimationclip.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QClipAnimatorPrivate;

class Q_3DANIMATIONSHARED_EXPORT QClipAnimator : public Qt3DAnimation::QAbstractClipAnimator
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAbstractAnimationClip *clip READ clip WRITE setClip NOTIFY clipChanged)

public:
    explicit QClipAnimator(Qt3DCore::QNode *parent = nullptr);
    ~QClipAnimator();

    Qt3DAnimation::QAbstractAnimationClip *clip() const;

public Q_SLOTS:
    void setClip(Qt3DAnimation::QAbstractAnimationClip *clip);

Q_SIGNALS:
    void clipChanged(Qt3DAnimation::QAbstractAnimationClip *clip);

protected:
    QClipAnimator(QClipAnimatorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QClipAnimator)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QCLIPANIMATOR_H
