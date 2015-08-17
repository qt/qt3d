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

#include "vsyncframeadvanceservice_p.h"
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>
#include <QSemaphore>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class VSyncFrameAdvanceServicePrivate Q_DECL_FINAL : public QAbstractFrameAdvanceServicePrivate
{
public:
    VSyncFrameAdvanceServicePrivate()
        : QAbstractFrameAdvanceServicePrivate(QStringLiteral("Renderer Aspect Frame Advance Service - aligned with vsync"))
        , m_semaphore(0)
        , m_elapsedTimeSincePreviousFrame(0)
    {
    }

    QSemaphore m_semaphore;
    QElapsedTimer m_elapsed;
    quint64 m_elapsedTimeSincePreviousFrame;
};

VSyncFrameAdvanceService::VSyncFrameAdvanceService()
    : QAbstractFrameAdvanceService(*new VSyncFrameAdvanceServicePrivate())
{
}

VSyncFrameAdvanceService::~VSyncFrameAdvanceService()
{
}

// Aspect Thread
qint64 VSyncFrameAdvanceService::waitForNextFrame()
{
    Q_D(VSyncFrameAdvanceService);
    d->m_semaphore.acquire(1);

    const quint64 currentTime = d->m_elapsed.nsecsElapsed();
    qCDebug(VSyncAdvanceService) << "Elapsed nsecs since last call " << currentTime - d->m_elapsedTimeSincePreviousFrame;
    d->m_elapsedTimeSincePreviousFrame = currentTime;
    return currentTime;
}

void VSyncFrameAdvanceService::start()
{
    Q_D(VSyncFrameAdvanceService);
    d->m_elapsed.start();
}

/*!
 \internal

 Stops the VSyncFrameAdvanceService, releases the semaphore to allow the
 renderer to cleanup without having to call waitForNextFrame.
 */
void VSyncFrameAdvanceService::stop()
{
    Q_D(VSyncFrameAdvanceService);
    if (d->m_semaphore.available() == 0)
        d->m_semaphore.release(1);
    qCDebug(VSyncAdvanceService) << "Terminating VSyncFrameAdvanceService";
}

// Render Thread
void VSyncFrameAdvanceService::proceedToNextFrame()
{
    Q_D(VSyncFrameAdvanceService);
    d->m_semaphore.release(1);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
