// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtickclock_p.h"

#include <QtCore/QThread>

#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QTickClock::QTickClock()
    : m_tickInterval(1000000000 / 60) // Nanoseconds
    , m_time(0)
{
}

void QTickClock::setTickFrequency(float frequency)
{
    Q_ASSERT(frequency > 0.0f);
    m_tickInterval = static_cast<qint64>(1000000000 / frequency);
}

void QTickClock::start()
{
    m_timer.start();
    m_time = m_timer.nsecsElapsed();
    qCDebug(ChangeArbiter) << "tickInterval =" << m_tickInterval << "ns";
}

void QTickClock::reset()
{
    m_time = 0;
}

qint64 QTickClock::waitForNextTick()
{
    qint64 t = m_timer.nsecsElapsed();
    qint64 dt = t - m_time;
    qint64 timeToSleep = m_tickInterval - dt;

    // If we are lagging behind don't delay
    // TODO: Do we want a mode where if we are lagging we instead wait
    // for the next tick instead?
    if (timeToSleep < 0) {
        qCDebug(ChangeArbiter) << "Lagging behind desired tick interval";
        m_time = t;
        return m_time;
    }

    unsigned long sleepTimeMicroSeconds = static_cast<unsigned long>(timeToSleep / 1000);
    //qCDebug(ChangeArbiter) << "sleeping for" << sleepTimeMicroSeconds << "us";
    QThread::usleep(sleepTimeMicroSeconds);

#if defined(QT3DCORE_DEBUG_TICKCLOCK)
    qint64 expectedWakeTime = t + sleepTimeMicroSeconds * 1000;
    qint64 wakeTime = m_timer.nsecsElapsed();
    qCDebug(ChangeArbiter) << "t =" << t / 1000000 << "timeToSleep =" << timeToSleep / 1000000
             << "due to wake at =" << expectedWakeTime / 1000000
             << "actually woke at" << wakeTime / 1000000
             << "delta =" << (expectedWakeTime - wakeTime) / 1000000;
#endif

    m_time = m_timer.nsecsElapsed();
    return m_time;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
