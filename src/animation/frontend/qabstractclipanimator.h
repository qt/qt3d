// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QABSTRACTCLIPANIMATOR_H
#define QT3DANIMATION_QABSTRACTCLIPANIMATOR_H

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/qclock.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationClip;
class QAbstractClipAnimatorPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAbstractClipAnimator : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(Qt3DAnimation::QChannelMapper *channelMapper READ channelMapper WRITE setChannelMapper NOTIFY channelMapperChanged)
    Q_PROPERTY(Qt3DAnimation::QClock *clock READ clock WRITE setClock NOTIFY clockChanged)
    Q_PROPERTY(float normalizedTime READ normalizedTime WRITE setNormalizedTime NOTIFY normalizedTimeChanged)

public:
    enum Loops { Infinite = -1 };
    Q_ENUM(Loops)

    ~QAbstractClipAnimator();

    bool isRunning() const;
    Qt3DAnimation::QChannelMapper *channelMapper() const;
    int loopCount() const;
    Qt3DAnimation::QClock *clock() const;
    float normalizedTime() const;

public Q_SLOTS:
    void setRunning(bool running);
    void setChannelMapper(Qt3DAnimation::QChannelMapper *channelMapper);
    void setLoopCount(int loops);
    void setClock(Qt3DAnimation::QClock *clock);
    void setNormalizedTime(float timeFraction);

    void start();
    void stop();

Q_SIGNALS:
    void runningChanged(bool running);
    void channelMapperChanged(Qt3DAnimation::QChannelMapper *channelMapper);
    void loopCountChanged(int loops);
    void clockChanged(Qt3DAnimation::QClock *clock);
    void normalizedTimeChanged(float index);

protected:
    explicit QAbstractClipAnimator(Qt3DCore::QNode *parent = nullptr);
    QAbstractClipAnimator(QAbstractClipAnimatorPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractClipAnimator)
};

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QABSTRACTCLIPANIMATOR_H
