// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtickclockservice_p.h"

#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p_p.h>
#include <Qt3DCore/private/qtickclock_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QTickClockServicePrivate : public QAbstractFrameAdvanceServicePrivate
{
public:
    QTickClockServicePrivate()
        : QAbstractFrameAdvanceServicePrivate(QStringLiteral("Default Frame Advance Service implementation"))
    {
        m_clock.setTickFrequency(60);
        m_clock.start();
    }

    QTickClock m_clock;
};

/* !\internal
    \class Qt3DCore::QTickClockService
    \inmodule Qt3DCore
    \brief Default QAbstractFrameAdvanceService implementation.

    This default QAbstractFrameAdvanceService implementation has a frequency of 60 Hz.
*/
QTickClockService::QTickClockService()
    : QAbstractFrameAdvanceService(*new QTickClockServicePrivate())
{
}

QTickClockService::~QTickClockService()
{
}

qint64 QTickClockService::waitForNextFrame()
{
    Q_D(QTickClockService);
    return d->m_clock.waitForNextTick();
}

/*
    Starts the inner tick clock used by the service.
 */
void QTickClockService::start()
{
    Q_D(QTickClockService);
    d->m_clock.start();
}

void QTickClockService::stop()
{
}

} // Qt3D

QT_END_NAMESPACE

#include "moc_qtickclockservice_p.cpp"
