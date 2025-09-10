// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QANIMATIONCLIP_H
#define QT3DANIMATION_QANIMATIONCLIP_H

#include <Qt3DAnimation/qabstractanimationclip.h>
#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qanimationclipdata.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClipPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAnimationClip : public QAbstractAnimationClip
{
    Q_OBJECT
    Q_PROPERTY(Qt3DAnimation::QAnimationClipData clipData READ clipData WRITE setClipData NOTIFY clipDataChanged)

public:
    explicit QAnimationClip(Qt3DCore::QNode *parent = nullptr);
    ~QAnimationClip();

    Qt3DAnimation::QAnimationClipData clipData() const;

public Q_SLOTS:
    void setClipData(const Qt3DAnimation::QAnimationClipData &clipData);

Q_SIGNALS:
    void clipDataChanged(Qt3DAnimation::QAnimationClipData clipData);

protected:
    QAnimationClip(QAnimationClipPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAnimationClip)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCLIP_H
