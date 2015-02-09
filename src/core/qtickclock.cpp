/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtickclock_p.h"

#include <Qt3DCore/private/corelogging_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

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

#if defined(QT3D_DEBUG_TICKCLOCK)
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

} // namespace Qt3D

QT_END_NAMESPACE
