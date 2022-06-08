// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
