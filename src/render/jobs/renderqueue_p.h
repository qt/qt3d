/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RENDERQUEUE_H
#define QT3DRENDER_RENDER_RENDERQUEUE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <vector>
#include <QMutex>
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

template<class RenderView>
class RenderQueue
{
public:
    RenderQueue()
        : m_noRender(false)
        , m_wasReset(true)
        , m_targetRenderViewCount(0)
        , m_currentRenderViewCount(0)
        , m_currentWorkQueue(1)
    {}

    /*
     Sets the number \a targetRenderViewCount of RenderView objects that make
     up a frame.
     */
    void setTargetRenderViewCount(int targetRenderViewCount)
    {
        Q_ASSERT(!m_noRender);
        m_targetRenderViewCount = targetRenderViewCount;
        m_currentWorkQueue.resize(targetRenderViewCount);
        m_wasReset = false;
    }

    inline int targetRenderViewCount() const { return m_targetRenderViewCount; }
    inline int currentRenderViewCount() const { return m_currentRenderViewCount; }

    /*
     Returns true if all the RenderView objects making up the current frame
     have been queued. Returns false otherwise.
     \note a frameQueue or size 0 is considered incomplete.
     */
    inline bool isFrameQueueComplete() const
    {
        return (m_noRender
                || (m_targetRenderViewCount > 0 && m_targetRenderViewCount == m_currentRenderViewCount));
    }

    /*
     Queue up a RenderView for the frame being built.
     Thread safe as this is called from the renderer which is locked.
     Returns true if the renderView is complete
     */
    bool queueRenderView(RenderView *renderView, uint submissionOrderIndex)
    {
        Q_ASSERT(!m_noRender);
        m_currentWorkQueue[submissionOrderIndex] = renderView;
        ++m_currentRenderViewCount;
        Q_ASSERT(m_currentRenderViewCount <= m_targetRenderViewCount);
        return isFrameQueueComplete();
    }

    /*
     Called by the Rendering Thread to retrieve the a frame queue to render.
     A call to reset is required after rendering of the frame. Otherwise under some
     conditions the current but then invalidated frame queue could be reused.
     */
    inline const std::vector<RenderView *> &nextFrameQueue() const { return m_currentWorkQueue; }

    /*
     In case the framegraph changed or when the current number of render queue
     needs to be reset.
     */
    void reset()
    {
        m_currentRenderViewCount = 0;
        m_targetRenderViewCount = 0;
        Qt3DCore::deleteAll(m_currentWorkQueue);
        m_currentWorkQueue.clear();
        m_noRender = false;
        m_wasReset = true;
    }

    void setNoRender()
    {
        Q_ASSERT(m_targetRenderViewCount == 0);
        m_noRender = true;
    }
    inline bool isNoRender() const { return m_noRender; }

    inline bool wasReset() const { return m_wasReset; }

    inline QMutex *mutex() { return &m_mutex; }

private:
    bool m_noRender;
    bool m_wasReset;
    int m_targetRenderViewCount;
    int m_currentRenderViewCount;
    std::vector<RenderView *> m_currentWorkQueue;
    QMutex m_mutex;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERQUEUE_H
