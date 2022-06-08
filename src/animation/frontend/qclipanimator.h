// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
