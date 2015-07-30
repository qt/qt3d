/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qtickclockservice_p.h"
#include "qtickclock_p.h"
#include "qabstractframeadvanceservice_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
/*!
    \class Qt3D::QTickClockServicePrivate
    \internal
*/

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

/*!
    \class Qt3D::QTickClockService
    \inmodule Qt3DCore
    \brief Default Qt3D::QAbstractFrameAdvanceService implementation.

    This default Qt3D::QAbstractFrameAdvanceService implementation has a frequency of 60 Hz.
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

/*!
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
