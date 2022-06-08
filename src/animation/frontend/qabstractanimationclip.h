// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTANIMATIONCLIP_H
#define QT3DANIMATION_QABSTRACTANIMATIONCLIP_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractAnimationClipPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAbstractAnimationClip : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float duration READ duration NOTIFY durationChanged)

public:
    ~QAbstractAnimationClip();

    float duration() const;

Q_SIGNALS:
    void durationChanged(float duration);

protected:
    QAbstractAnimationClip(QAbstractAnimationClipPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractAnimationClip)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTANIMATIONCLIP_H
