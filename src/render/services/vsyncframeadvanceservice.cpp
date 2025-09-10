// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "vsyncframeadvanceservice_p.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p_p.h>
#include <QSemaphore>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class VSyncFrameAdvanceServicePrivate final : public Qt3DCore::QAbstractFrameAdvanceServicePrivate
{
public:
    explicit VSyncFrameAdvanceServicePrivate(bool drivenByRenderThread)
        : QAbstractFrameAdvanceServicePrivate(QStringLiteral("Renderer Aspect Frame Advance Service - aligned with vsync"))
        , m_semaphore(0)
        , m_elapsedTimeSincePreviousFrame(0)
        , m_drivenByRenderThread(drivenByRenderThread)
    {
    }

    QSemaphore m_semaphore;
    QElapsedTimer m_elapsed;
    qint64 m_elapsedTimeSincePreviousFrame;
    bool m_drivenByRenderThread;
};

VSyncFrameAdvanceService::VSyncFrameAdvanceService(bool drivenByRenderThread)
    : QAbstractFrameAdvanceService(*new VSyncFrameAdvanceServicePrivate(drivenByRenderThread))
{
}

VSyncFrameAdvanceService::~VSyncFrameAdvanceService()
{
}

// Main Thread
qint64 VSyncFrameAdvanceService::waitForNextFrame()
{
    Q_D(VSyncFrameAdvanceService);

    d->m_semaphore.acquire(std::max(d->m_semaphore.available(), 1));

    const qint64 currentTime = d->m_elapsed.nsecsElapsed();
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
    d->m_semaphore.release(1);
    qCDebug(VSyncAdvanceService) << "Terminating VSyncFrameAdvanceService";
}

// Render Thread
void VSyncFrameAdvanceService::proceedToNextFrame()
{
    Q_D(VSyncFrameAdvanceService);
    d->m_semaphore.release(1);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
