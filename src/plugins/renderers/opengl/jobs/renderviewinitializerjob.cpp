/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 Paul Lemire
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

#include "renderviewinitializerjob_p.h"

#include <renderview_p.h>
#include <renderer_p.h>
#include <renderviewjobutils_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/job_common_p.h>

#include <QElapsedTimer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

namespace {
// only accessed in ctor and dtor of RenderViewJob
// which are always being called in a non concurrent manner
int renderViewInstanceCounter = 0;
} // anonymous

RenderViewInitializerJob::RenderViewInitializerJob()
    : m_renderer(nullptr)
    , m_fgLeaf(nullptr)
    , m_index(0)
    , m_renderView(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderView, renderViewInstanceCounter++)
}

RenderViewInitializerJob::~RenderViewInitializerJob()
{
    renderViewInstanceCounter--;
}

void RenderViewInitializerJob::run()
{
    qCDebug(Jobs) << Q_FUNC_INFO << m_index;
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    QElapsedTimer timer;
    timer.start();
    qint64 gatherLightsTime;
    qint64 buildCommandsTime;
#endif

    // Create a RenderView object
    // The RenderView are created from a QFrameAllocator stored in the current Thread local storage
    m_renderView = new RenderView;

    // RenderView should allocate heap resources using only the currentFrameAllocator
    m_renderView->setRenderer(m_renderer);

    // Populate the renderview's configuration from the framegraph
    setRenderViewConfigFromFrameGraphLeafNode(m_renderView, m_fgLeaf);
#if defined(QT3D_RENDER_VIEW_JOB_TIMINGS)
    qint64 gatherStateTime = timer.nsecsElapsed();
    timer.restart();
#endif
}

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
