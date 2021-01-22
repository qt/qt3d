/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "renderqueue_p.h"
#include <renderview_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RenderQueue::RenderQueue()
    : m_noRender(false),
      m_wasReset(true),
      m_targetRenderViewCount(0),
      m_currentRenderViewCount(0),
      m_currentWorkQueue(1)
{
}

int RenderQueue::currentRenderViewCount() const
{
    return m_currentRenderViewCount;
}

/*
 * In case the framegraph changed or when the current number of render queue
 * needs to be reset.
 */
void RenderQueue::reset()
{
    m_currentRenderViewCount = 0;
    m_targetRenderViewCount = 0;
    m_currentWorkQueue.clear();
    m_noRender = false;
    m_wasReset = true;
}

void RenderQueue::setNoRender()
{
    Q_ASSERT(m_targetRenderViewCount == 0);
    m_noRender = true;
}

/*
 * Queue up a RenderView for the frame being built.
 * Thread safe as this is called from the renderer which is locked.
 * Returns true if the renderView is complete
 */
bool RenderQueue::queueRenderView(RenderView *renderView, uint submissionOrderIndex)
{
    Q_ASSERT(!m_noRender);
    m_currentWorkQueue[submissionOrderIndex] = renderView;
    ++m_currentRenderViewCount;
    Q_ASSERT(m_currentRenderViewCount <= m_targetRenderViewCount);
    return isFrameQueueComplete();
}

/*
 * Called by the Rendering Thread to retrieve the a frame queue to render.
 * A call to reset is required after rendering of the frame. Otherwise under some
 * conditions the current but then invalidated frame queue could be reused.
 */
QVector<RenderView *> RenderQueue::nextFrameQueue()
{
    return m_currentWorkQueue;
}

/*
 * Sets the number \a targetRenderViewCount of RenderView objects that make up a frame.
 */
void RenderQueue::setTargetRenderViewCount(int targetRenderViewCount)
{
    Q_ASSERT(!m_noRender);
    m_targetRenderViewCount = targetRenderViewCount;
    m_currentWorkQueue.resize(targetRenderViewCount);
    m_wasReset = false;
}

/*
 * Returns true if all the RenderView objects making up the current frame have been queued.
 * Returns false otherwise.
 * \note a frameQueue or size 0 is considered incomplete.
 */
bool RenderQueue::isFrameQueueComplete() const
{
    return (m_noRender
            || (m_targetRenderViewCount > 0
                && m_targetRenderViewCount == m_currentRenderViewCount));
}

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
