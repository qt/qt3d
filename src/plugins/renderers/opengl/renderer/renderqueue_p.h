/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERQUEUE_H
#define QT3DRENDER_RENDER_OPENGL_RENDERQUEUE_H

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

#include <QVector>
#include <QtGlobal>
#include <QMutex>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

class RenderView;

class Q_AUTOTEST_EXPORT RenderQueue
{
public:
    RenderQueue();

    void setTargetRenderViewCount(int targetRenderViewCount);
    int targetRenderViewCount() const { return m_targetRenderViewCount; }
    int currentRenderViewCount() const;
    bool isFrameQueueComplete() const;

    bool queueRenderView(RenderView *renderView, uint submissionOrderIndex);
    QVector<RenderView *> nextFrameQueue();
    void reset();

    void setNoRender();
    inline bool isNoRender() const { return m_noRender; }

    inline bool wasReset() const { return m_wasReset; }

    inline QMutex *mutex() { return &m_mutex; }

private:
    bool m_noRender;
    bool m_wasReset;
    int m_targetRenderViewCount;
    int m_currentRenderViewCount;
    QVector<RenderView *> m_currentWorkQueue;
    QMutex m_mutex;
};

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERQUEUE_H
