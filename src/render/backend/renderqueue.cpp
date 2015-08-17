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

#include "renderqueue_p.h"
#include <Qt3DRenderer/private/renderview_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderQueue::RenderQueue()
    : m_targetRenderViewCount(0)
    , m_currentRenderViewCount(0)
    , m_currentWorkQueue(1)
{
}

int RenderQueue::currentRenderViewCount() const
{
    return m_currentRenderViewCount;
}

/*!
 * In case the framegraph changed or when the current number of render queue
 * needs to be reset.
 */
void RenderQueue::reset()
{
    m_currentRenderViewCount = 0;
    Q_ASSERT(currentRenderViewCount() == 0);
}

/*!
 * Queue up a RenderView for the frame being built.
 * Thread safe as this is called from the renderer which is locked.
 * Returns true if the renderView is complete
 */
bool RenderQueue::queueRenderView(RenderView *renderView, uint submissionOrderIndex)
{
    m_currentWorkQueue[submissionOrderIndex] = renderView;
    ++m_currentRenderViewCount;
    return isFrameQueueComplete();
}

/*!
 * Called by the Rendering Thread to retrieve the a frame queue to render.
 * A call to reset is required after rendering of the frame. Otherwise under some
 * conditions the current but then invalidated frame queue could be reused.
 */
QVector<RenderView *> RenderQueue::nextFrameQueue()
{
    return m_currentWorkQueue;
}

/*!
 * Sets the number \a targetRenderViewCount of RenderView objects that make up a frame.
 */
void RenderQueue::setTargetRenderViewCount(int targetRenderViewCount)
{
    m_targetRenderViewCount = targetRenderViewCount;
    m_currentWorkQueue.resize(targetRenderViewCount);
}

/*!
 * Returns true if all the RenderView objects making up the current frame have been queued.
 * Returns false otherwise.
 * \note a frameQueue or size 0 is considered incomplete.
 */
bool RenderQueue::isFrameQueueComplete() const
{
    return m_targetRenderViewCount && m_targetRenderViewCount == currentRenderViewCount();
}

} // Render

} // Qt3D

QT_END_NAMESPACE
