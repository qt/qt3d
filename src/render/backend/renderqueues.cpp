/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderqueues.h"
#include "renderview.h"
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderQueues::RenderQueues(int capacity) :
    m_targetRenderViewCount(1),
    m_queues(capacity),
    m_currentWorkQueue(1)
{
    m_currentRenderViewCount.fetchAndStoreOrdered(0);
}

int RenderQueues::currentRenderViewCount() const
{
    return m_currentRenderViewCount.loadAcquire();
}

/*!
 * In case framegraph changed we have to be able to externally
 * reset the RenderQueues
 */
void RenderQueues::reset()
{
    m_currentRenderViewCount.fetchAndStoreOrdered(0);
    Q_ASSERT(currentRenderViewCount() == 0);
    QMutexLocker locker(&m_mutex);
    m_queues.clear();
}

/*!
 * Queue up a RenderView for the frame being built.
 * Thread safe as long as we're sure that 2 threads won't use the same submissionOrderIndex.
 */
void RenderQueues::queueRenderView(RenderView *renderView, uint submissionOrderIndex)
{
    m_currentWorkQueue[submissionOrderIndex] = renderView;
    m_currentRenderViewCount.ref();
}

/*!
 * Sets the number \a targetRenderViewCount of RenderView objects that make up a frame.
 */
void RenderQueues::setTargetRenderViewCount(int targetRenderViewCount)
{
    m_targetRenderViewCount = targetRenderViewCount;
    m_currentWorkQueue.resize(targetRenderViewCount);
}

/*!
 * Returns true if all the RenderView objects making up the current frame have been queued.
 * Returns false otherwise.
 */
bool RenderQueues::isFrameQueueComplete() const
{
    return m_targetRenderViewCount == currentRenderViewCount();
}

/*!
 * Called by the Rendering Thread to perform the rendering of the
 *  first frame queue stored.
 */
QVector<RenderView *> RenderQueues::popFrameQueue()
{
    return m_queues.pop_front();
}

/*!
 * Called by RenderViewJobs to push the current frame queue when all jobs
 * are done.
 */
void RenderQueues::pushFrameQueue()
{
    // Only one RenderViewJob should be able to switch to the next work queue
    // It should cost less to have the mutex here and make a second check of timeToSubmit
    // Than habing the QMutexLocker in the Renderer enqueueRenderView for each timeToSubmit
    QMutexLocker locker(&m_mutex);
    if (isFrameQueueComplete()) {
        m_currentRenderViewCount.fetchAndStoreOrdered(0);
        Q_ASSERT(currentRenderViewCount() == 0);
        locker.unlock();
        m_queues.push_back(QVector<RenderView *>(m_currentWorkQueue));
        for (int i = 0; i < m_currentWorkQueue.size(); i++)
            m_currentWorkQueue[i] = 0;
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
