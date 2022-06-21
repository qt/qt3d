// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qclock.h"
#include "qclock_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QClockPrivate::QClockPrivate()
    : Qt3DCore::QNodePrivate()
    , m_playbackRate(1.0f)
{
}

QClock::QClock(Qt3DCore::QNode* parent)
    : Qt3DCore::QNode(*new QClockPrivate, parent)
{
}

QClock::QClock(QClockPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(dd, parent)
{
}

QClock::~QClock()
{
}
/*!
    \property Qt3DAnimation::QClock::playbackRate

    The playback speed of the animation. The playback speed can be negative.
    When that is the case the animation will be played back from the current
    normalized time value back to 0 and for the number of loops it had been
    played for with a positive playback rate.
*/

double QClock::playbackRate() const
{
    Q_D(const QClock);
    return d->m_playbackRate;
}

void QClock::setPlaybackRate(double playbackRate)
{
    Q_D(QClock);
    if (qFuzzyCompare(playbackRate, d->m_playbackRate))
        return;
    d->m_playbackRate = playbackRate;
    emit playbackRateChanged(playbackRate);
}

}

QT_END_NAMESPACE

#include "moc_qclock.cpp"
